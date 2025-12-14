#include "mesh_systems.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "mesh_systems";

EventGroupHandle_t mesh_event_group = NULL;
bool mesh_connected = false;
bool mesh_is_root = false;
uint8_t parent_mac[6] = {0};
int mesh_layer = -1;

void mesh_systems_init(void)
{
    if (mesh_event_group == NULL)
    {
        mesh_event_group = xEventGroupCreate();
    }
}

void mesh_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    switch (id)
    {
    case MESH_EVENT_STARTED:
        mesh_connected = false;
        mesh_is_root = false;
        mesh_layer = esp_mesh_get_layer();
        memset(parent_mac, 0, sizeof(parent_mac));
        xEventGroupSetBits(mesh_event_group, MESH_EVT_STARTED);
        ESP_LOGI(TAG, "Mesh started, layer %d", mesh_layer);
        break;

    case MESH_EVENT_PARENT_CONNECTED:
    {
        mesh_event_connected_t *event = (mesh_event_connected_t *)data;
        mesh_connected = true;
        mesh_is_root = esp_mesh_is_root();
        mesh_layer = event->connected.self.layer;
        memcpy(parent_mac, event->connected.parent.addr, sizeof(parent_mac));
        xEventGroupSetBits(mesh_event_group, MESH_EVT_PARENT_CONNECTED);
        ESP_LOGI(TAG, "Parent connected: layer %d, parent=" MACSTR ", root=%d",
                 mesh_layer, MAC2STR(parent_mac), mesh_is_root);
        break;
    }

    case MESH_EVENT_PARENT_DISCONNECTED:
        mesh_connected = false;
        mesh_layer = esp_mesh_get_layer();
        memset(parent_mac, 0, sizeof(parent_mac));
        xEventGroupSetBits(mesh_event_group, MESH_EVT_PARENT_DISCONNECTED);
        ESP_LOGW(TAG, "Parent disconnected; attempting to rejoin mesh");
        break;

    case MESH_EVENT_CHILD_CONNECTED:
    {
        mesh_event_child_connected_t *child = (mesh_event_child_connected_t *)data;
        xEventGroupSetBits(mesh_event_group, MESH_EVT_CHILD_CONNECTED);
        ESP_LOGI(TAG, "Child connected, aid=%d, mac=" MACSTR,
                 child->aid, MAC2STR(child->mac));
        break;
    }

    case MESH_EVENT_CHILD_DISCONNECTED:
    {
        mesh_event_child_disconnected_t *child = (mesh_event_child_disconnected_t *)data;
        xEventGroupSetBits(mesh_event_group, MESH_EVT_CHILD_DISCONNECTED);
        ESP_LOGW(TAG, "Child disconnected, aid=%d, mac=" MACSTR,
                 child->aid, MAC2STR(child->mac));
        break;
    }

    case MESH_EVENT_ROOT_GOT_IP:
        xEventGroupSetBits(mesh_event_group, MESH_EVT_ROOT_GOT_IP);
        ESP_LOGI(TAG, "Root obtained IP address");
        break;

    default:
        break;
    }
}
