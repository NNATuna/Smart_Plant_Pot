#include "utils_common.h"
#include "esp_log.h"
#include "esp_random.h"

int utils_backoff_ms(int attempt, int base_ms, int max_ms, int jitter_ms)
{
    if (attempt < 0) {
        attempt = 0;
    }
    int backoff = base_ms * (1 << (attempt > 10 ? 10 : attempt));
    if (backoff > max_ms) {
        backoff = max_ms;
    }
    uint32_t jitter = jitter_ms > 0 ? esp_random() % jitter_ms : 0;
    backoff += jitter;
    ESP_LOGD("utils", "backoff attempt=%d delay=%dms", attempt, backoff);
    return backoff;
}
