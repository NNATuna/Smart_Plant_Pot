#pragma once

#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int utils_backoff_ms(int attempt, int base_ms, int max_ms, int jitter_ms);

#ifdef __cplusplus
}
#endif
