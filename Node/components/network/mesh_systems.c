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
    if (esp_mesh_get_parent_bssid(&parent) == ESP_OK) {
        memcpy(s_parent_mac, parent.addr, sizeof(s_parent_mac));
    }
}

static void mesh_systems_update_root(void)
{
    mesh_addr_t root = {0};
    if (esp_mesh_get_root_id(&root) == ESP_OK) {
        memcpy(s_root_mac, root.addr, sizeof(s_root_mac));
    } else if (s_is_root) {
        esp_wifi_get_mac(WIFI_IF_STA, s_root_mac);
    }
}

void mesh_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    switch (id) {
    case MESH_EVENT_STARTED:
        s_connected = false;
        s_mesh_layer = esp_mesh_get_layer();
        memset(s_parent_mac, 0, sizeof(s_parent_mac));
        s_is_root = esp_mesh_is_root();
        mesh_systems_update_root();
        ESP_LOGI(SYS_TAG, "Mesh started");
        break;
    case MESH_EVENT_PARENT_CONNECTED:
        s_mesh_layer = esp_mesh_get_layer();
        s_is_root = esp_mesh_is_root();
        s_connected = true;
        mesh_systems_update_parent();
        mesh_systems_update_root();
        ESP_LOGI(SYS_TAG, "Parent connected, layer %d, root %d", s_mesh_layer, s_is_root);
        break;
    case MESH_EVENT_PARENT_DISCONNECTED:
        s_connected = false;
        s_mesh_layer = esp_mesh_get_layer();
        memset(s_parent_mac, 0, sizeof(s_parent_mac));
        ESP_LOGW(SYS_TAG, "Parent disconnected");
        break;
    case MESH_EVENT_ROOT_GOT_IP:
        s_connected = true;
        s_is_root = true;
        mesh_systems_update_root();
        ESP_LOGI(SYS_TAG, "Root obtained IP");
        break;
    case MESH_EVENT_ROOT_LOST_IP:
        s_connected = false;
        ESP_LOGW(SYS_TAG, "Root lost IP");
        break;
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
