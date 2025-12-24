#pragma once

#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MESH_EVENT_ROOT_GOT_IP,
    MESH_EVENT_ROOT_LOST_IP,
    MESH_EVENT_CHILD_CONNECTED,
    MESH_EVENT_CHILD_DISCONNECTED,
    MESH_EVENT_PARENT_CHANGED,
} mesh_normalized_event_t;

typedef void (*mesh_rx_callback_t)(const uint8_t *data, size_t len, int8_t rssi, uint8_t layer);

typedef void (*mesh_event_callback_t)(mesh_normalized_event_t event);

esp_err_t mesh_if_init(mesh_event_callback_t event_cb);
esp_err_t mesh_if_start(void);
esp_err_t mesh_if_send_to_node(const uint8_t *addr, const uint8_t *data, size_t len);
esp_err_t mesh_if_register_rx_callback(mesh_rx_callback_t cb);

#ifdef __cplusplus
}
#endif
