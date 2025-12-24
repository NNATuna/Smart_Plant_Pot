#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_log.h"
#include "app_config.h"
#include "runtime_role.h"
#include "mesh_if.h"
#include "pipeline.h"
#include "storage_spool.h"
#include "uplink.h"
#include "cloud.h"
#include "registry.h"
#include "sdkconfig.h"

static const char *TAG = "app_main";
static pipeline_context_t s_pipeline_ctx = {0};

static void mesh_event_router(mesh_normalized_event_t event)
{
    runtime_role_on_mesh_event(event);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Gateway/Relay skeleton booting");
    app_config_t config = {0};
    ESP_ERROR_CHECK(app_config_init(&config));
    ESP_ERROR_CHECK(runtime_role_init(CONFIG_CAN_UPLINK));
    ESP_ERROR_CHECK(mesh_if_init(mesh_event_router));
    ESP_ERROR_CHECK(pipeline_init(&s_pipeline_ctx));
    ESP_ERROR_CHECK(registry_init());
    ESP_ERROR_CHECK(spool_init());
    ESP_ERROR_CHECK(mesh_if_start());
    ESP_ERROR_CHECK(pipeline_start(&s_pipeline_ctx));

    if (runtime_role_can_uplink()) {
        wifi_sta_init(config.wifi_ssid, config.wifi_password);
        time_sync_init();
        cloud_client_init(config.cloud_base_url, config.cloud_auth_token);
        cloud_sync_start();
        cloud_config_pull_start();
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
