#include "mesh_protocol.h"
#include "mesh_manager.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MESH_PROTOCOL_TAG "mesh_protocol"
#define SENSOR_SEND_INTERVAL_MS (5 * 60 * 1000)
#define SENSOR_POLL_INTERVAL_MS (30 * 1000)
#define SOIL_THRESHOLD 2.0f
#define TEMP_THRESHOLD 0.5f
#define HUMIDITY_THRESHOLD 1.0f
#define LUX_THRESHOLD 10.0f

static mesh_protocol_control_cb_t control_cb = NULL;
static char node_id_str[18] = {0};
static bool protocol_started = false;

static void mesh_protocol_rx_handler(const uint8_t *data, size_t len, const mesh_addr_t *from)
{
    cJSON *root = cJSON_ParseWithLength((const char *)data, len);
    if (!root)
    {
        ESP_LOGW(MESH_PROTOCOL_TAG, "Failed to parse incoming JSON");
        return;
    }

    cJSON *command = cJSON_GetObjectItem(root, "command");
    if (command && cJSON_IsString(command) && control_cb)
    {
        control_cb(command->valuestring, root);
    }
    else
    {
        char *printed = cJSON_PrintUnformatted(root);
        ESP_LOGI(MESH_PROTOCOL_TAG, "Received message from " MACSTR ": %s",
                 MAC2STR(from->addr), printed ? printed : "<invalid>");
        cJSON_free(printed);
    }

    cJSON_Delete(root);
}

static char *mesh_protocol_build_payload(const sensor_data_t *data)
{
    cJSON *root = cJSON_CreateObject();
    if (!root)
    {
        return NULL;
    }

    cJSON_AddStringToObject(root, "node_id", node_id_str);
    cJSON_AddNumberToObject(root, "soil", data->soil_percent);
    cJSON_AddNumberToObject(root, "temperature", data->temperature);
    cJSON_AddNumberToObject(root, "humidity", data->humidity);
    cJSON_AddNumberToObject(root, "water_level", data->soil_mv);
    cJSON_AddNumberToObject(root, "lux", data->lux);

    char *payload = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return payload;
}

static bool mesh_protocol_should_send(const sensor_data_t *current, const sensor_data_t *last)
{
    if (last == NULL)
    {
        return true;
    }

    if (fabsf(current->soil_percent - last->soil_percent) >= SOIL_THRESHOLD)
    {
        return true;
    }
    if (fabsf(current->temperature - last->temperature) >= TEMP_THRESHOLD)
    {
        return true;
    }
    if (fabsf(current->humidity - last->humidity) >= HUMIDITY_THRESHOLD)
    {
        return true;
    }
    if (fabsf(current->lux - last->lux) >= LUX_THRESHOLD)
    {
        return true;
    }

    return false;
}

static void mesh_protocol_tx_task(void *arg)
{
    sensor_data_t last_sent = {0};
    bool has_last = false;
    TickType_t last_time = xTaskGetTickCount();

    while (1)
    {
        sensor_data_t current = {0};
        sensor_manager_read(&current);

        bool should_send = mesh_protocol_should_send(&current, has_last ? &last_sent : NULL);
        TickType_t now = xTaskGetTickCount();
        if ((now - last_time) >= pdMS_TO_TICKS(SENSOR_SEND_INTERVAL_MS))
        {
            should_send = true;
        }

        if (should_send)
        {
            if (mesh_protocol_send_sensor_data(&current) == ESP_OK)
            {
                last_sent = current;
                has_last = true;
                last_time = now;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_POLL_INTERVAL_MS));
    }
}

void mesh_protocol_init(void)
{
    if (protocol_started)
    {
        return;
    }

    uint8_t mac[6] = {0};
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    snprintf(node_id_str, sizeof(node_id_str), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    mesh_transport_register_rx_callback(mesh_protocol_rx_handler);
    mesh_transport_init();

    xTaskCreate(mesh_protocol_tx_task, "mesh_proto_tx", 4096, NULL, 5, NULL);

    protocol_started = true;
}

esp_err_t mesh_protocol_send_sensor_data(const sensor_data_t *data)
{
    if (!data)
    {
        return ESP_ERR_INVALID_ARG;
    }

    char *payload = mesh_protocol_build_payload(data);
    if (!payload)
    {
        return ESP_ERR_NO_MEM;
    }

    size_t len = strlen(payload);
    esp_err_t err = mesh_transport_send((uint8_t *)payload, len);
    if (err != ESP_OK)
    {
        ESP_LOGE(MESH_PROTOCOL_TAG, "Failed to queue sensor payload: %s", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(MESH_PROTOCOL_TAG, "Sensor data queued (%d bytes)", (int)len);
    }

    free(payload);
    return err;
}

void mesh_protocol_set_control_callback(mesh_protocol_control_cb_t cb)
{
    control_cb = cb;
}

