#include "pipeline.h"
#include "esp_log.h"
#include "cJSON.h"
#include "storage_spool.h"
#include "registry.h"
#include "sdkconfig.h"
#include <string.h>

static const char *TAG = "pipeline";
static pipeline_context_t *s_ctx = NULL;

static void mesh_rx_adapter(const uint8_t *data, size_t len, int8_t rssi, uint8_t layer)
{
    if (!s_ctx || !s_ctx->raw_queue) {
        return;
    }
    raw_frame_t frame = {0};
    size_t copy_len = len > sizeof(frame.data) ? sizeof(frame.data) : len;
    memcpy(frame.data, data, copy_len);
    frame.len = copy_len;
    frame.rssi = rssi;
    frame.layer = layer;
    if (xQueueSend(s_ctx->raw_queue, &frame, 0) != pdPASS) {
        utils_metrics_inc(s_ctx->metrics.lock, &s_ctx->metrics.drop_count);
        ESP_LOGW(TAG, "raw_queue full, dropping oldest frame");
    }
}

static bool decode_frame(const raw_frame_t *frame, message_envelope_t *out)
{
    if (!frame || !out) {
        return false;
    }
    /* Minimal placeholder decoder: assumes JSON text already framed */
    out->payload_len = frame->len > sizeof(out->payload) ? sizeof(out->payload) : frame->len;
    memcpy(out->payload, frame->data, out->payload_len);
    out->rssi = (uint8_t)frame->rssi;
    out->layer = frame->layer;
    out->seq = 0;
    out->schema_version = 1;
    out->payload_type = PAYLOAD_TYPE_SENSOR;
    return true;
}

static void decoder_task(void *arg)
{
    pipeline_context_t *ctx = (pipeline_context_t *)arg;
    raw_frame_t frame;
    message_envelope_t packet;
    while (1) {
        if (xQueueReceive(ctx->raw_queue, &frame, portMAX_DELAY) == pdTRUE) {
            if (decode_frame(&frame, &packet)) {
                registry_update_last_seen(packet.node_id, packet.seq, packet.rssi, packet.layer);
                if (xQueueSend(ctx->decoded_queue, &packet, pdMS_TO_TICKS(10)) != pdPASS) {
                    utils_metrics_inc(ctx->metrics.lock, &ctx->metrics.drop_count);
                    ESP_LOGW(TAG, "decoded_queue full");
                }
            }
        }
    }
}

static void spool_task(void *arg)
{
    pipeline_context_t *ctx = (pipeline_context_t *)arg;
    message_envelope_t packet;
    while (1) {
        if (xQueueReceive(ctx->decoded_queue, &packet, portMAX_DELAY) == pdTRUE) {
            spool_record_t rec = {
                .seq = packet.seq,
                .payload_len = packet.payload_len,
            };
            memcpy(rec.node_id, packet.node_id, sizeof(rec.node_id));
            memcpy(rec.payload, packet.payload, packet.payload_len);
            if (spool_append(&rec) == ESP_OK) {
                utils_metrics_inc(ctx->metrics.lock, &ctx->metrics.processed_count);
            }
        }
    }
}

esp_err_t pipeline_init(pipeline_context_t *ctx)
{
    if (!ctx) {
        return ESP_ERR_INVALID_ARG;
    }
    s_ctx = ctx;
    utils_metrics_init(&ctx->metrics);
    ctx->raw_queue = xQueueCreate(CONFIG_RAW_QUEUE_LEN, sizeof(raw_frame_t));
    ctx->decoded_queue = xQueueCreate(CONFIG_DECODED_QUEUE_LEN, sizeof(message_envelope_t));
    ctx->storage_queue = xQueueCreate(CONFIG_STORAGE_QUEUE_LEN, sizeof(message_envelope_t));
    if (!ctx->raw_queue || !ctx->decoded_queue || !ctx->storage_queue) {
        ESP_LOGE(TAG, "Failed to create queues");
        return ESP_ERR_NO_MEM;
    }
    mesh_if_register_rx_callback(mesh_rx_adapter);
    return ESP_OK;
}

esp_err_t pipeline_start(pipeline_context_t *ctx)
{
    if (!ctx) {
        return ESP_ERR_INVALID_ARG;
    }
    xTaskCreate(decoder_task, "decoder", 4096, ctx, 6, NULL);
    xTaskCreate(spool_task, "spool", 4096, ctx, 5, NULL);
    ESP_LOGI(TAG, "pipeline tasks started");
    return ESP_OK;
}

esp_err_t pipeline_submit_raw(const uint8_t *data, size_t len, int8_t rssi, uint8_t layer)
{
    if (!s_ctx || !s_ctx->raw_queue) {
        return ESP_ERR_INVALID_STATE;
    }
    mesh_rx_adapter(data, len, rssi, layer);
    return ESP_OK;
}
