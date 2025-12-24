#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t drop_count;
    uint32_t retry_count;
    uint32_t processed_count;
    SemaphoreHandle_t lock;
} metrics_counters_t;

void utils_metrics_init(metrics_counters_t *counters);
void utils_metrics_inc(SemaphoreHandle_t lock, uint32_t *counter);

#ifdef __cplusplus
}
#endif
