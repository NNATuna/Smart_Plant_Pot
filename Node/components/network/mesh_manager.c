#include "mesh_manager.h"
#include "mesh_systems.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <string.h>

static const char *TAG = "mesh_manager";

static void wifi_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());

    esp_err_t loop_ret = esp_event_loop_create_default();
    if (loop_ret != ESP_OK && loop_ret != ESP_ERR_INVALID_STATE)
    {
        ESP_ERROR_CHECK(loop_ret);
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    ESP_LOGI(TAG, "Wi-Fi initialized for mesh use");
}

static void mesh_configure_and_start(void)
{
    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();

    uint8_t mesh_id[6] = {
        CONFIG_SMARTPOT_MESH_ID0,
        CONFIG_SMARTPOT_MESH_ID1,
        CONFIG_SMARTPOT_MESH_ID2,
        CONFIG_SMARTPOT_MESH_ID3,
        CONFIG_SMARTPOT_MESH_ID4,
        CONFIG_SMARTPOT_MESH_ID5,
    };

    memcpy(cfg.mesh_id, mesh_id, sizeof(mesh_id));

    cfg.channel = CONFIG_SMARTPOT_MESH_CHANNEL;
    cfg.router.ssid_len = strlen(CONFIG_SMARTPOT_MESH_ROUTER_SSID);
    memcpy(cfg.router.ssid, CONFIG_SMARTPOT_MESH_ROUTER_SSID, cfg.router.ssid_len);
    memcpy(cfg.router.password, CONFIG_SMARTPOT_MESH_ROUTER_PASS,
           strlen(CONFIG_SMARTPOT_MESH_ROUTER_PASS));

    cfg.mesh_ap.max_connection = CONFIG_SMARTPOT_MESH_AP_CONNECTIONS;
    memcpy(cfg.mesh_ap.password, CONFIG_SMARTPOT_MESH_AP_PASSWD,
           strlen(CONFIG_SMARTPOT_MESH_AP_PASSWD));

    ESP_ERROR_CHECK(esp_mesh_set_max_layer(CONFIG_SMARTPOT_MESH_MAX_LAYER));
    ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(CONFIG_SMARTPOT_MESH_VOTE_PERCENTAGE));
    ESP_ERROR_CHECK(esp_mesh_set_xon_qsize(CONFIG_SMARTPOT_MESH_XON_QSIZE));
    ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(10));

    ESP_ERROR_CHECK(esp_mesh_set_type(MESH_NODE));
    ESP_ERROR_CHECK(esp_mesh_set_topology(MESH_TOPO_TREE));
    ESP_ERROR_CHECK(esp_mesh_set_allow_root_conflicts(true));
    ESP_ERROR_CHECK(esp_mesh_set_self_organized(true, true));

    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, mesh_event_handler, NULL));

    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
    ESP_ERROR_CHECK(esp_mesh_start());

    ESP_LOGI(TAG, "Mesh started: layer=%d", esp_mesh_get_layer());
}

esp_err_t mesh_manager_init(void)
{
    mesh_systems_init();
    wifi_init();

    ESP_ERROR_CHECK(esp_mesh_init());
    mesh_configure_and_start();

    return ESP_OK;
}

bool mesh_manager_is_root(void)
{
    return mesh_is_root;
}

void mesh_manager_get_parent(mesh_addr_t *parent)
{
    if (parent == NULL)
    {
        return;
    }
    memcpy(parent->addr, parent_mac, sizeof(parent_mac));
}

int mesh_manager_get_layer(void)
{
    return mesh_layer;
}

