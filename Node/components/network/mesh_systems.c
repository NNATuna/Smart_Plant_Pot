#include "mesh_systems.h"

#include <string.h>
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_wifi.h"

#define SYS_TAG "mesh_systems"

static bool s_is_root = false;
static bool s_connected = false;
static int s_mesh_layer = -1;
static uint8_t s_parent_mac[6] = {0};
static uint8_t s_root_mac[6] = {0};

static void mesh_systems_update_parent(void)
{
    mesh_addr_t parent = {0};
    if (esp_mesh_get_parent_bssid(&parent) == ESP_OK)
    {
        memcpy(s_parent_mac, parent.addr, 6);
    }
}

void mesh_event_handler(void *arg, esp_event_base_t base, int32_t id, void *event_data)
{
    switch (id)
    {
    case MESH_EVENT_STARTED:
        s_connected = false;
        s_mesh_layer = esp_mesh_get_layer();
        memset(s_parent_mac, 0, sizeof(s_parent_mac));
        memset(s_root_mac, 0, sizeof(s_root_mac));
        s_is_root = false;
        ESP_LOGI(SYS_TAG, "Mesh started");
        break;

    case MESH_EVENT_PARENT_CONNECTED:
        s_mesh_layer = esp_mesh_get_layer();
        s_connected = true;
        s_is_root = esp_mesh_is_root();
        mesh_systems_update_parent();
        ESP_LOGI(SYS_TAG, "Parent connected, layer %d, root %d",
                 s_mesh_layer, s_is_root);
        break;

    case MESH_EVENT_PARENT_DISCONNECTED:
        s_connected = false;
        s_is_root = false;
        memset(s_parent_mac, 0, sizeof(s_parent_mac));
        ESP_LOGW(SYS_TAG, "Parent disconnected");
        break;

    case MESH_EVENT_ROOT_FIXED:
    {
        mesh_event_root_fixed_t *info =
            (mesh_event_root_fixed_t *)event_data;

        if (info->is_fixed)
        {
            s_is_root = true;
            esp_wifi_get_mac(WIFI_IF_STA, s_root_mac);

            ESP_LOGI(SYS_TAG, "Node is ROOT");
        }
        break;
    }

    default:
        break;
    }
}

bool mesh_systems_is_root(void)
{
    return s_is_root;
}

int mesh_systems_get_layer(void)
{
    return s_mesh_layer;
}

bool mesh_systems_is_connected(void)
{
    return s_connected;
}

void mesh_systems_get_parent_mac(uint8_t mac[6])
{
    memcpy(mac, s_parent_mac, 6);
}

void mesh_systems_get_root_mac(uint8_t mac[6])
{
    memcpy(mac, s_root_mac, 6);
}
