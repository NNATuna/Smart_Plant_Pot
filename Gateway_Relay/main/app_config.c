#include "app_config.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <string.h>

static const char *TAG = "app_config";
static app_config_t s_config;

esp_err_t app_config_init(app_config_t *config)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        return ret;
    }
    memset(&s_config, 0, sizeof(s_config));
    strlcpy(s_config.wifi_ssid, "YOUR_SSID", sizeof(s_config.wifi_ssid));
    strlcpy(s_config.wifi_password, "YOUR_PASS", sizeof(s_config.wifi_password));
    strlcpy(s_config.cloud_base_url, "https://example.firebaseio.com", sizeof(s_config.cloud_base_url));
    strlcpy(s_config.cloud_auth_token, "token", sizeof(s_config.cloud_auth_token));
    s_config.batch_time_sec = CONFIG_CLOUD_BATCH_TIME_SEC;
    s_config.batch_max_records = CONFIG_CLOUD_BATCH_MAX_RECORDS;
    if (config) {
        memcpy(config, &s_config, sizeof(s_config));
    }
    ESP_LOGI(TAG, "Config initialized");
    return ESP_OK;
}

const app_config_t *app_config_get(void)
{
    return &s_config;
}
