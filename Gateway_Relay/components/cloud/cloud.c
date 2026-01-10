#include "cloud.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "utils_common.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "cloud";
static char s_base_url[128];
static char s_auth_token[128];

static esp_err_t cloud_send_batch(const spool_record_t *records, int count)
{
    /* Placeholder: would serialize to JSON/CBOR and POST via esp_http_client */
    ESP_LOGI(TAG, "Sending batch count=%d", count);
    (void)records;
    return ESP_OK;
}

static void cloud_sync_task(void *arg)
{
    (void)arg;
    int attempt = 0;
    spool_record_t buffer[CONFIG_CLOUD_BATCH_MAX_RECORDS];
    spool_batch_info_t info = {0};
    while (1) {
        info.bytes_read = 0;
        info.records_read = 0;
        if (spool_read_batch(CONFIG_CLOUD_BATCH_MAX_RECORDS, CONFIG_CLOUD_BATCH_MAX_RECORDS * sizeof(spool_record_t), buffer, &info) == ESP_OK && info.records_read > 0) {
            esp_err_t ret = cloud_send_batch(buffer, info.records_read);
            if (ret == ESP_OK) {
                spool_commit(info.offset);
                attempt = 0;
            } else {
                int delay_ms = utils_backoff_ms(attempt++, 500, 10000, 250);
                vTaskDelay(pdMS_TO_TICKS(delay_ms));
                continue;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(CONFIG_CLOUD_BATCH_TIME_SEC * 1000));
    }
}

static void config_pull_task(void *arg)
{
    (void)arg;
    while (1) {
        ESP_LOGD(TAG, "Polling for config updates (stub)");
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

esp_err_t cloud_client_init(const char *base_url, const char *auth_token)
{
    if (base_url) {
        strlcpy(s_base_url, base_url, sizeof(s_base_url));
    }
    if (auth_token) {
        strlcpy(s_auth_token, auth_token, sizeof(s_auth_token));
    }
    return ESP_OK;
}

esp_err_t cloud_sync_start(void)
{
    xTaskCreate(cloud_sync_task, "cloud_sync", 4096, NULL, 5, NULL);
    return ESP_OK;
}

esp_err_t cloud_config_pull_start(void)
{
    xTaskCreate(config_pull_task, "config_pull", 4096, NULL, 4, NULL);
    return ESP_OK;
}
