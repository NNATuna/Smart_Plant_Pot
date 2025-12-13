#include "mesh_manager.h"
#include "mesh_systems.h"
#include "esp_err.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

const char *TAG = "wifi_manager";

void wifi_init(void)
{
    // NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Network interfaces
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Wi-Fi init
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Để mesh tự set mode
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi initialized for mesh");
}
void mesh_init(void)
{
    ESP_ERROR_CHECK(esp_mesh_init());

    // Cấu hình basic
    ESP_ERROR_CHECK(esp_mesh_set_max_layer(CONFIG_ESP_MESH_MAX_LAYER));
    ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(1));
    ESP_ERROR_CHECK(esp_mesh_set_xon_qsize(64));

    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();

    // ----- Mesh ID -----
    memcpy(cfg.mesh_id, mesh_id, 6);

    // ----- Channel -----
    cfg.channel = CONFIG_ESP_MESH_CHANNEL;

    // ----- Router (root AP) -----
    cfg.router.ssid_len = strlen(CONFIG_ESP_MESH_ROUTER_SSID);
    memcpy(cfg.router.ssid, CONFIG_ESP_MESH_ROUTER_SSID, cfg.router.ssid_len);
    memcpy(cfg.router.password, CONFIG_ESP_MESH_ROUTER_PASS,
           strlen(CONFIG_ESP_MESH_ROUTER_PASS));

    // ----- Mesh Type -----
    ESP_ERROR_CHECK(esp_mesh_set_type(MESH_NODE));

    // Đăng ký event handler
    esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, mesh_event_handler, NULL);

    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
    ESP_ERROR_CHECK(esp_mesh_start());

    ESP_LOGI(TAG, "Mesh node started");
}
void mesh_init(void)
{
    ESP_ERROR_CHECK(esp_mesh_init());

    // Cấu hình basic
    ESP_ERROR_CHECK(esp_mesh_set_max_layer(CONFIG_ESP_MESH_MAX_LAYER));
    ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(1));
    ESP_ERROR_CHECK(esp_mesh_set_xon_qsize(64));

    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();

    // ----- Mesh ID -----
    memcpy(cfg.mesh_id, mesh_id, 6);

    // ----- Channel -----
    cfg.channel = CONFIG_ESP_MESH_CHANNEL;

    // ----- Router (root AP) -----
    cfg.router.ssid_len = strlen(CONFIG_ESP_MESH_SSID);
    memcpy(cfg.router.ssid, CONFIG_ESP_MESH_SSID, cfg.router.ssid_len);
    memcpy(cfg.router.password, CONFIG_ESP_MESH_PASSWORD,
           strlen(CONFIG_ESP_MESH_PASSWORD));

    // ----- Mesh Type -----
    ESP_ERROR_CHECK(esp_mesh_set_type(MESH_NODE));

    // Đăng ký event handler
    esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, mesh_event_handler, NULL);

    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
    ESP_ERROR_CHECK(esp_mesh_start());

    ESP_LOGI(TAG, "Mesh node started");
}
