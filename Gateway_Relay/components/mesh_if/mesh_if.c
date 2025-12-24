#include "mesh_if.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char *TAG = "mesh_if";
static mesh_rx_callback_t s_rx_cb = NULL;
static mesh_event_callback_t s_event_cb = NULL;

static void mesh_internal_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    if (base != MESH_EVENT) {
        return;
    }
    switch (id) {
        case MESH_EVENT_ROOT_GOT_IP:
            if (s_event_cb) s_event_cb(MESH_EVENT_ROOT_GOT_IP);
            break;
        case MESH_EVENT_ROOT_LOST_IP:
            if (s_event_cb) s_event_cb(MESH_EVENT_ROOT_LOST_IP);
            break;
        case MESH_EVENT_CHILD_CONNECTED:
            if (s_event_cb) s_event_cb(MESH_EVENT_CHILD_CONNECTED);
            break;
        case MESH_EVENT_CHILD_DISCONNECTED:
            if (s_event_cb) s_event_cb(MESH_EVENT_CHILD_DISCONNECTED);
            break;
        case MESH_EVENT_PARENT_CONNECTED:
        case MESH_EVENT_PARENT_DISCONNECTED:
            if (s_event_cb) s_event_cb(MESH_EVENT_PARENT_CHANGED);
            break;
        default:
            break;
    }
}

static void mesh_data_rx_task(void *arg)
{
    /* In production, this would block on esp_mesh_recv(). For skeleton, simulate idle loop. */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        (void)arg;
    }
}

esp_err_t mesh_if_init(mesh_event_callback_t event_cb)
{
    s_event_cb = event_cb;
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_wifi_init(&(wifi_init_config_t)ESP_WIFI_INIT_CONFIG_DEFAULT()));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, mesh_internal_event_handler, NULL));
    return ESP_OK;
}

esp_err_t mesh_if_start(void)
{
    ESP_LOGI(TAG, "Starting mesh stack (stub)");
    xTaskCreate(mesh_data_rx_task, "mesh_rx", 4096, NULL, 5, NULL);
    return ESP_OK;
}

esp_err_t mesh_if_send_to_node(const uint8_t *addr, const uint8_t *data, size_t len)
{
    ESP_LOGD(TAG, "send to node stub len=%zu", len);
    (void)addr;
    (void)data;
    (void)len;
    return ESP_OK;
}

esp_err_t mesh_if_register_rx_callback(mesh_rx_callback_t cb)
{
    s_rx_cb = cb;
    ESP_LOGI(TAG, "Registered RX callback");
    return ESP_OK;
}
