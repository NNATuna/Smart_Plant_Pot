#include "mesh_transport.h"
#include "mesh_systems.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MESH_TX_QUEUE_LEN 10
#define MESH_TRANSPORT_TAG "mesh_transport"

typedef struct
{
    uint8_t *payload;
    size_t length;
} mesh_tx_item_t;

static QueueHandle_t tx_queue = NULL;
static mesh_rx_callback_t rx_callback = NULL;
static bool transport_started = false;

static void mesh_transport_tx_task(void *arg)
{
    mesh_tx_item_t item;
    mesh_addr_t dest = {0};

    while (1)
    {
        if (xQueueReceive(tx_queue, &item, portMAX_DELAY) == pdTRUE)
        {
            mesh_data_t data = {
                .data = item.payload,
                .size = item.length,
                .proto = MESH_PROTO_BIN,
                .tos = MESH_TOS_P2P,
            };

            esp_err_t err = esp_mesh_send(&dest, &data, MESH_DATA_P2P, NULL, 0);
            if (err != ESP_OK)
            {
                ESP_LOGE(MESH_TRANSPORT_TAG, "Failed to send mesh packet: %s", esp_err_to_name(err));
            }
            else
            {
                ESP_LOGD(MESH_TRANSPORT_TAG, "Mesh packet sent (%d bytes)", item.length);
            }

            free(item.payload);
        }
    }
}

static void mesh_transport_rx_task(void *arg)
{
    const int mtu = esp_mesh_get_mtu();
    uint8_t *rx_buf = malloc(mtu);
    if (!rx_buf)
    {
        ESP_LOGE(MESH_TRANSPORT_TAG, "RX buffer allocation failed");
        vTaskDelete(NULL);
        return;
    }

    mesh_addr_t from = {0};
    mesh_data_t data = {
        .data = rx_buf,
        .size = mtu,
        .proto = MESH_PROTO_BIN,
    };

    while (1)
    {
        int flag = 0;
        data.size = mtu;
        esp_err_t err = esp_mesh_recv(&from, &data, portMAX_DELAY, &flag, NULL, 0);
        if (err != ESP_OK)
        {
            ESP_LOGE(MESH_TRANSPORT_TAG, "Mesh receive failed: %s", esp_err_to_name(err));
            continue;
        }

        if (rx_callback)
        {
            rx_callback((uint8_t *)data.data, data.size, &from);
        }
    }
}

void mesh_transport_init(void)
{
    if (transport_started)
    {
        return;
    }

    if (tx_queue == NULL)
    {
        tx_queue = xQueueCreate(MESH_TX_QUEUE_LEN, sizeof(mesh_tx_item_t));
    }

    xTaskCreate(mesh_transport_tx_task, "mesh_tx", 4096, NULL, 5, NULL);
    xTaskCreate(mesh_transport_rx_task, "mesh_rx", 4096, NULL, 5, NULL);

    transport_started = true;
}

esp_err_t mesh_transport_send(const uint8_t *data, size_t len)
{
    if (!mesh_connected)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (tx_queue == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (!data || len == 0)
    {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t *buffer = malloc(len);
    if (!buffer)
    {
        return ESP_ERR_NO_MEM;
    }
    memcpy(buffer, data, len);

    mesh_tx_item_t item = {
        .payload = buffer,
        .length = len,
    };

    if (xQueueSend(tx_queue, &item, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        free(buffer);
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}

void mesh_transport_register_rx_callback(mesh_rx_callback_t cb)
{
    rx_callback = cb;
}

