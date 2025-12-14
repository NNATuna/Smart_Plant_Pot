#pragma once

#include "cJSON.h"
#include "esp_err.h"
#include "mesh_transport.h"
#include "sensors_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mesh_protocol_control_cb_t)(const char *command, const cJSON *payload);

void mesh_protocol_init(void);

esp_err_t mesh_protocol_send_sensor_data(const sensor_data_t *data);

void mesh_protocol_set_control_callback(mesh_protocol_control_cb_t cb);

#ifdef __cplusplus
}
#endif

