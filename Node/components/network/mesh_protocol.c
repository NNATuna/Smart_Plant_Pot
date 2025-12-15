#include "mesh_protocol.h"

#include <string.h>
#include "esp_log.h"
#include "esp_wifi.h"

#define PROTO_TAG "mesh_protocol"

typedef struct __attribute__((packed)) {
    uint8_t node_id[6];
    uint8_t message_type;
    uint16_t payload_length;
} mesh_protocol_header_t;

esp_err_t mesh_protocol_pack_sensor_data(const sensor_data_t *data,
                                         uint8_t message_type,
                                         uint8_t *out_buf,
                                         size_t buf_size,
                                         size_t *out_len)
{
    if (!data || !out_buf || !out_len) {
        return ESP_ERR_INVALID_ARG;
    }

    mesh_protocol_header_t header = {0};
    esp_wifi_get_mac(WIFI_IF_STA, header.node_id);
    header.message_type = message_type;
    header.payload_length = sizeof(sensor_data_t);

    size_t total_size = sizeof(header) + sizeof(sensor_data_t);
    if (buf_size < total_size) {
        return ESP_ERR_NO_MEM;
    }

    memcpy(out_buf, &header, sizeof(header));
    memcpy(out_buf + sizeof(header), data, sizeof(sensor_data_t));
    *out_len = total_size;

    ESP_LOGI(PROTO_TAG, "Packed sensor data len=%u", (unsigned)total_size);
    return ESP_OK;
}

esp_err_t mesh_protocol_unpack(const uint8_t *data,
                               size_t data_len,
                               mesh_protocol_packet_t *out_packet)
{
    if (!data || !out_packet) {
        return ESP_ERR_INVALID_ARG;
    }

    if (data_len < sizeof(mesh_protocol_header_t)) {
        return ESP_ERR_INVALID_SIZE;
    }

    mesh_protocol_header_t header = {0};
    memcpy(&header, data, sizeof(header));

    if (data_len < sizeof(header) + header.payload_length) {
        return ESP_ERR_INVALID_SIZE;
    }

    memcpy(out_packet->node_id, header.node_id, sizeof(header.node_id));
    out_packet->message_type = header.message_type;
    out_packet->payload_length = header.payload_length;
    out_packet->payload = data + sizeof(header);

    return ESP_OK;
}
