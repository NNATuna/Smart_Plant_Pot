#include "bridge.h"
#include "mesh_if.h"
#include "esp_log.h"

static const char *TAG = "bridge";

esp_err_t downlink_bridge_send_config_to_node(const uint8_t *node_addr, const uint8_t *data, size_t len)
{
    ESP_LOGI(TAG, "Sending config to node");
    return mesh_if_send_to_node(node_addr, data, len);
}
