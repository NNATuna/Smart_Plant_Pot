#include "mesh_manager.h"
#include "mesh_systems.h"

#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#define MESH_TAG "mesh_manager"

static const uint8_t s_mesh_id[MESH_ID_LEN] = {0x32, 0x10, 0x45, 0x78, 0x9A, 0xBC};

static esp_err_t mesh_manager_configure_mesh(void);

esp_err_t mesh_manager_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(MESH_TAG, "Wi-Fi initialized for mesh");
    return ESP_OK;
}

static esp_err_t mesh_manager_configure_mesh(void)
{
    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();

    memcpy((uint8_t *)cfg.mesh_id, s_mesh_id, MESH_ID_LEN);
    cfg.channel = CONFIG_ESP_MESH_CHANNEL;
    cfg.router.ssid_len = strlen(CONFIG_ESP_WIFI_SSID);
    memcpy(cfg.router.ssid, CONFIG_ESP_WIFI_SSID, cfg.router.ssid_len);
    memcpy(cfg.router.password, CONFIG_ESP_WIFI_PASSWORD, strlen(CONFIG_ESP_WIFI_PASSWORD));

    cfg.mesh_ap.max_connection = CONFIG_ESP_MESH_MAX_LAYER; // allow same as layer limit
    cfg.mesh_ap.authmode = WIFI_AUTH_WPA2_PSK;
    if (strlen(CONFIG_ESP_MESH_PASSWORD) == 0) {
        cfg.mesh_ap.authmode = WIFI_AUTH_OPEN;
    } else {
        memcpy(cfg.mesh_ap.password, CONFIG_ESP_MESH_PASSWORD, strlen(CONFIG_ESP_MESH_PASSWORD));
    }

    ESP_ERROR_CHECK(esp_mesh_set_max_layer(CONFIG_ESP_MESH_MAX_LAYER));
    ESP_ERROR_CHECK(esp_mesh_set_ap_authmode(cfg.mesh_ap.authmode));
    ESP_ERROR_CHECK(esp_mesh_set_leave_indication(true));
    ESP_ERROR_CHECK(esp_mesh_set_xon_qsize(128));
    ESP_ERROR_CHECK(esp_mesh_allow_root_conflicts(true));
    ESP_ERROR_CHECK(esp_mesh_set_self_organized(true, false));
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));

    return ESP_OK;
}

esp_err_t mesh_manager_start(void)
{
    ESP_ERROR_CHECK(esp_mesh_init());
    ESP_ERROR_CHECK(mesh_manager_configure_mesh());

    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, mesh_event_handler, NULL));

    ESP_ERROR_CHECK(esp_mesh_start());
    ESP_LOGI(MESH_TAG, "Mesh started");
    return ESP_OK;
}

esp_err_t mesh_manager_stop(void)
{
    ESP_ERROR_CHECK(esp_mesh_stop());
    ESP_ERROR_CHECK(esp_event_handler_unregister(MESH_EVENT, ESP_EVENT_ANY_ID, mesh_event_handler));
    ESP_LOGI(MESH_TAG, "Mesh stopped");
    return ESP_OK;
}

bool mesh_manager_is_root(void)
{
    return esp_mesh_is_root();
}
