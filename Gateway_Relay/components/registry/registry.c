#include "registry.h"
#include <string.h>
#include "esp_log.h"

#define REGISTRY_MAX_ENTRIES 32

static const char *TAG = "registry";
static registry_entry_t s_entries[REGISTRY_MAX_ENTRIES];
static int s_count = 0;

esp_err_t registry_init(void)
{
    memset(s_entries, 0, sizeof(s_entries));
    s_count = 0;
    return ESP_OK;
}

esp_err_t registry_update_last_seen(const uint8_t node_id[8], uint32_t seq, int8_t rssi, uint8_t layer)
{
    if (!node_id) {
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < s_count; ++i) {
        if (memcmp(s_entries[i].node_id, node_id, sizeof(s_entries[i].node_id)) == 0) {
            s_entries[i].last_seq = seq;
            s_entries[i].last_rssi = rssi;
            s_entries[i].last_layer = layer;
            return ESP_OK;
        }
    }
    if (s_count >= REGISTRY_MAX_ENTRIES) {
        ESP_LOGW(TAG, "registry full");
        return ESP_ERR_NO_MEM;
    }
    memcpy(s_entries[s_count].node_id, node_id, sizeof(s_entries[s_count].node_id));
    s_entries[s_count].last_seq = seq;
    s_entries[s_count].last_rssi = rssi;
    s_entries[s_count].last_layer = layer;
    s_count++;
    return ESP_OK;
}

int registry_get_entries(registry_entry_t *out, int max_entries)
{
    if (!out || max_entries <= 0) {
        return 0;
    }
    int copy = s_count < max_entries ? s_count : max_entries;
    memcpy(out, s_entries, copy * sizeof(registry_entry_t));
    return copy;
}
