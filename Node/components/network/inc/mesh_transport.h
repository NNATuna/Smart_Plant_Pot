#ifndef MESH_TRANSPORT_H
#define MESH_TRANSPORT_H

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t mesh_transport_send_to_root(const uint8_t *data, size_t length);
void mesh_transport_receive_task(void *param);

#ifdef __cplusplus
}
#endif

#endif // MESH_TRANSPORT_H
