#pragma once

#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t downlink_bridge_send_config_to_node(const uint8_t *node_addr, const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif
