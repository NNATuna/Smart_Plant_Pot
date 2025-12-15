#ifndef MESH_SYSTEMS_H
#define MESH_SYSTEMS_H

#include <stdbool.h>
#include <stdint.h>
#include "esp_event.h"

#ifdef __cplusplus
extern "C" {
#endif

void mesh_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data);
bool mesh_systems_is_root(void);
int mesh_systems_get_layer(void);
bool mesh_systems_is_connected(void);
void mesh_systems_get_parent_mac(uint8_t mac[6]);
void mesh_systems_get_root_mac(uint8_t mac[6]);

#ifdef __cplusplus
}
#endif

#endif // MESH_SYSTEMS_H
