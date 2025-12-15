#ifndef MESH_PROTOCOL_H
#define MESH_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "sensors_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t node_id[6];
    uint8_t message_type;
    uint16_t payload_length;
    const uint8_t *payload;
} mesh_protocol_packet_t;

esp_err_t mesh_protocol_pack_sensor_data(const sensor_data_t *data,
                                         uint8_t message_type,
                                         uint8_t *out_buf,
                                         size_t buf_size,
                                         size_t *out_len);

esp_err_t mesh_protocol_unpack(const uint8_t *data,
                               size_t data_len,
                               mesh_protocol_packet_t *out_packet);

#ifdef __cplusplus
}
#endif

#endif // MESH_PROTOCOL_H
