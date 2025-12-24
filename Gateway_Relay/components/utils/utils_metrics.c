#include "utils_metrics.h"
#include "esp_log.h"

void utils_metrics_init(metrics_counters_t *counters)
{
    if (!counters) {
        return;
    }
    counters->drop_count = 0;
    counters->retry_count = 0;
    counters->processed_count = 0;
    counters->lock = xSemaphoreCreateMutex();
    if (!counters->lock) {
        ESP_LOGE("metrics", "Failed to create metrics mutex");
    }
}

void utils_metrics_inc(SemaphoreHandle_t lock, uint32_t *counter)
{
    if (!counter) {
        return;
    }
    if (lock) {
        if (xSemaphoreTake(lock, pdMS_TO_TICKS(10)) == pdTRUE) {
            (*counter)++;
            xSemaphoreGive(lock);
        }
    } else {
        (*counter)++;
    }
}
