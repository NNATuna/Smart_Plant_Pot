#include "mesh_transport.h"
#include "mesh_protocol.h"
#include "mesh_systems.h"

#include <string.h>
#include "esp_log.h"
#include "esp_mesh.h"
#include "freertos/FreeRTOS.h"

#define TRANS_TAG "mesh_transport"

esp_err_t mesh_transport_send_to_root(const uint8_t *data, size_t length)
{
    if (!data || length == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (mesh_systems_is_root()) {
        ESP_LOGI(TRANS_TAG, "Node is root, handling data locally");
        return ESP_OK;
    }

    mesh_addr_t root = {0};
    ESP_ERROR_CHECK(esp_mesh_get_root_id(&root));

    mesh_data_t mesh_data = {
        .data = (uint8_t *)data,
        .size = length,
        .proto = MESH_PROTO_BIN,
        .tos = MESH_TOS_P2P,
    };

    esp_err_t err = esp_mesh_send(&root, &mesh_data, 0, NULL, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TRANS_TAG, "Failed to send to root: %s", esp_err_to_name(err));
    }
    return err;
}

void mesh_transport_receive_task(void *param)
{
    mesh_addr_t from = {0};
    uint8_t rx_buf[512];
    mesh_data_t data = {
        .data = rx_buf,
        .size = sizeof(rx_buf),
    };
    int flag = 0;

    while (1) {
        data.size = sizeof(rx_buf);
        esp_err_t err = esp_mesh_recv(&from, &data, portMAX_DELAY, &flag, NULL);
        if (err != ESP_OK) {
            ESP_LOGE(TRANS_TAG, "Receive error: %s", esp_err_to_name(err));
            continue;
        }

        mesh_protocol_packet_t packet = {0};
        if (mesh_protocol_unpack(rx_buf, data.size, &packet) == ESP_OK) {
            ESP_LOGI(TRANS_TAG, "Received msg type %u from %02X:%02X:%02X:%02X:%02X:%02X len %u", 
                     packet.message_type,
                     packet.node_id[0], packet.node_id[1], packet.node_id[2],
                     packet.node_id[3], packet.node_id[4], packet.node_id[5],
                     packet.payload_length);
        } else {
            ESP_LOGW(TRANS_TAG, "Received undecodable packet size %u", (unsigned)data.size);
        }
    }
}
