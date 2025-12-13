#include "mesh_systems.h"
#include "esp_log.h"

static const char *TAG = "mesh_systems";

EventGroupHandle_t mesh_event_group;

bool mesh_connected = false;
uint8_t parent_mac[6];
int mesh_layer = -1;

void mesh_event_handler(void *arg, esp_event_base_t base,
                        int32_t id, void *data)
{
    switch (id)
    {
    case ESP_MESH_LITE_EVENT_NODE_JOIN:
        ESP_LOGI(TAG, "Node joined mesh");
        xEventGroupSetBits(mesh_event_group, MESH_EVT_NODE_JOINED);
        break;

    case ESP_MESH_LITE_EVENT_NODE_LEAVE:
        ESP_LOGI(TAG, "Node left mesh");
        xEventGroupSetBits(mesh_event_group, MESH_EVT_NODE_LEFT);
        break;

    case ESP_MESH_LITE_EVENT_NODE_CHANGE:
        ESP_LOGI(TAG, "Mesh topology changed");

        mesh_connected = esp_mesh_lite_is_connected();
        mesh_layer = esp_mesh_lite_get_level();
        esp_mesh_lite_get_parent_addr(parent_mac);

        xEventGroupSetBits(mesh_event_group, MESH_EVT_NODE_CHANGED);

        ESP_LOGI(TAG, "Connected=%d Layer=%d Parent=" MACSTR,
                 mesh_connected, mesh_layer, MAC2STR(parent_mac));
        break;
    }
}
