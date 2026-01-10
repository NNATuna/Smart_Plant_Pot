#pragma once

#include <stddef.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "mesh_if.h"
#include "utils_metrics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PAYLOAD_TYPE_SENSOR = 0,
    PAYLOAD_TYPE_DIAGNOSTIC = 1,
    PAYLOAD_TYPE_CONFIG_ACK = 2,
} payload_type_t;

typedef struct {
    uint8_t node_id[8];
    uint32_t seq;
    uint64_t ts_ms;
    uint16_t schema_version;
    payload_type_t payload_type;
    uint8_t rssi;
    uint8_t layer;
    uint16_t crc16;
    size_t payload_len;
    uint8_t payload[256];
} message_envelope_t;

typedef struct {
    uint8_t data[256];
    size_t len;
    int8_t rssi;
    uint8_t layer;
} raw_frame_t;

typedef struct {
    QueueHandle_t raw_queue;
    QueueHandle_t decoded_queue;
    QueueHandle_t storage_queue;
    metrics_counters_t metrics;
} pipeline_context_t;

esp_err_t pipeline_init(pipeline_context_t *ctx);
esp_err_t pipeline_start(pipeline_context_t *ctx);
esp_err_t pipeline_submit_raw(const uint8_t *data, size_t len, int8_t rssi, uint8_t layer);

#ifdef __cplusplus
}
#endif
