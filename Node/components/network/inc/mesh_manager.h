#pragma once

#include "esp_err.h"
#include "esp_mesh.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Wi-Fi and ESP-MESH stack.
 *
 * This function will initialize NVS, network interfaces, Wi-Fi, register
 * the mesh event handler, configure the mesh parameters and start the mesh
 * networking stack.
 *
 * @return ESP_OK on success, otherwise an error from ESP-IDF APIs.
 */
esp_err_t mesh_manager_init(void);

/**
 * @brief Determine if the current node is acting as the mesh root.
 */
bool mesh_manager_is_root(void);

/**
 * @brief Get the parent MAC address for the current node.
 *
 * @param parent Buffer to copy the parent address into.
 */
void mesh_manager_get_parent(mesh_addr_t *parent);

/**
 * @brief Get the current mesh layer of this node.
 */
int mesh_manager_get_layer(void);

#ifdef __cplusplus
}
#endif

/* -------------------- Configuration defaults -------------------- */
#ifndef CONFIG_SMARTPOT_MESH_AP_PASSWD
#define CONFIG_SMARTPOT_MESH_AP_PASSWD "meshnode"
#endif

#ifndef CONFIG_SMARTPOT_MESH_AP_CONNECTIONS
#define CONFIG_SMARTPOT_MESH_AP_CONNECTIONS 6
#endif

#ifndef CONFIG_SMARTPOT_MESH_ID0
#define CONFIG_SMARTPOT_MESH_ID0 0x7D
#endif
#ifndef CONFIG_SMARTPOT_MESH_ID1
#define CONFIG_SMARTPOT_MESH_ID1 0xB2
#endif
#ifndef CONFIG_SMARTPOT_MESH_ID2
#define CONFIG_SMARTPOT_MESH_ID2 0x3F
#endif
#ifndef CONFIG_SMARTPOT_MESH_ID3
#define CONFIG_SMARTPOT_MESH_ID3 0x4A
#endif
#ifndef CONFIG_SMARTPOT_MESH_ID4
#define CONFIG_SMARTPOT_MESH_ID4 0xA5
#endif
#ifndef CONFIG_SMARTPOT_MESH_ID5
#define CONFIG_SMARTPOT_MESH_ID5 0x5C
#endif

#ifndef CONFIG_SMARTPOT_MESH_ROUTER_SSID
#define CONFIG_SMARTPOT_MESH_ROUTER_SSID ""
#endif

#ifndef CONFIG_SMARTPOT_MESH_ROUTER_PASS
#define CONFIG_SMARTPOT_MESH_ROUTER_PASS ""
#endif

#ifndef CONFIG_SMARTPOT_MESH_CHANNEL
#define CONFIG_SMARTPOT_MESH_CHANNEL CONFIG_ESP_MESH_CHANNEL
#endif

#ifndef CONFIG_SMARTPOT_MESH_MAX_LAYER
#define CONFIG_SMARTPOT_MESH_MAX_LAYER CONFIG_ESP_MESH_MAX_LAYER
#endif

#ifndef CONFIG_SMARTPOT_MESH_VOTE_PERCENTAGE
#define CONFIG_SMARTPOT_MESH_VOTE_PERCENTAGE 1
#endif

#ifndef CONFIG_SMARTPOT_MESH_XON_QSIZE
#define CONFIG_SMARTPOT_MESH_XON_QSIZE 64
#endif

