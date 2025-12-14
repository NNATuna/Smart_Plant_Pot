#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_mesh.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mesh_rx_callback_t)(const uint8_t *data, size_t len, const mesh_addr_t *from);

void mesh_transport_init(void);

esp_err_t mesh_transport_send(const uint8_t *data, size_t len);

void mesh_transport_register_rx_callback(mesh_rx_callback_t cb);

#ifdef __cplusplus
}
#endif

