#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t node_id[8];
    uint32_t last_seq;
    int8_t last_rssi;
    uint8_t last_layer;
} registry_entry_t;

esp_err_t registry_init(void);
esp_err_t registry_update_last_seen(const uint8_t node_id[8], uint32_t seq, int8_t rssi, uint8_t layer);
int registry_get_entries(registry_entry_t *out, int max_entries);

#ifdef __cplusplus
}
#endif
