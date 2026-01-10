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

static const uint8_t s_mesh_id[6] = {0x32, 0x10, 0x45, 0x78, 0x9A, 0xBC};

static esp_err_t mesh_manager_configure_mesh(void);
static void log_mesh_channel(void)
{
    uint8_t primary;
    wifi_second_chan_t second;

    esp_err_t err = esp_wifi_get_channel(&primary, &second);
    if (err == ESP_OK)
    {
        ESP_LOGI("MESH", "Mesh channel (WiFi AP): %d, second: %d",
                 primary, second);
    }
    else
    {
        ESP_LOGW("MESH", "Failed to get mesh channel: %s",
                 esp_err_to_name(err));
    }
}
static void log_router_channel(void)
{
    if (!esp_mesh_is_root())
    {
        ESP_LOGI("ROOT", "Not root, skip router channel log");
        return;
    }

    wifi_ap_record_t ap;
    esp_err_t err = esp_wifi_sta_get_ap_info(&ap);
    if (err == ESP_OK)
    {
        ESP_LOGI("ROOT", "Router channel (STA): %d, SSID: %s",
                 ap.primary, (char *)ap.ssid);
    }
    else
    {
        ESP_LOGW("ROOT", "Router not connected yet: %s",
                 esp_err_to_name(err));
    }
}

esp_err_t mesh_manager_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_t *netif_sta = NULL;
    esp_netif_t *netif_ap = NULL;
    ESP_ERROR_CHECK(
        esp_netif_create_default_wifi_mesh_netifs(&netif_sta, &netif_ap));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(MESH_TAG, "Wi-Fi initialized for mesh");
    return ESP_OK;
}

static esp_err_t mesh_manager_configure_mesh(void)
{
    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();

    memcpy(cfg.mesh_id.addr, s_mesh_id, 6);

    cfg.channel = CONFIG_ESP_MESH_CHANNEL;

    cfg.router.ssid_len = strlen(CONFIG_ESP_WIFI_SSID);
    memcpy(cfg.router.ssid,
           CONFIG_ESP_WIFI_SSID,
           cfg.router.ssid_len);
    memcpy(cfg.router.password,
           CONFIG_ESP_WIFI_PASSWORD,
           strlen(CONFIG_ESP_WIFI_PASSWORD));

    cfg.mesh_ap.max_connection = 6;

    ESP_ERROR_CHECK(esp_mesh_set_max_layer(CONFIG_ESP_MESH_MAX_LAYER));
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

    ESP_ERROR_CHECK(
        esp_event_handler_register(MESH_EVENT,
                                   ESP_EVENT_ANY_ID,
                                   mesh_event_handler,
                                   NULL));

    ESP_ERROR_CHECK(esp_mesh_start());

    esp_err_t err = esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20);
    if (err != ESP_OK)
    {
        ESP_LOGW(MESH_TAG, "AP HT20 not applied yet: %s", esp_err_to_name(err));
    }
    log_mesh_channel();
    log_router_channel();
    ESP_LOGI(MESH_TAG, "Mesh started");
    return ESP_OK;
}

esp_err_t mesh_manager_stop(void)
{
    ESP_ERROR_CHECK(esp_mesh_stop());
    ESP_ERROR_CHECK(
        esp_event_handler_unregister(MESH_EVENT,
                                     ESP_EVENT_ANY_ID,
                                     mesh_event_handler));
    ESP_LOGI(MESH_TAG, "Mesh stopped");
    return ESP_OK;
}

bool mesh_manager_is_root(void)
{
    return esp_mesh_is_root();
}
