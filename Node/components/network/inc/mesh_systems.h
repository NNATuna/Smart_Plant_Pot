#pragma once

#include "esp_event.h"
#include "esp_mesh.h"
#include "freertos/event_groups.h"
#include <stdbool.h>

#define MESH_EVT_STARTED BIT0
#define MESH_EVT_PARENT_CONNECTED BIT1
#define MESH_EVT_PARENT_DISCONNECTED BIT2
#define MESH_EVT_CHILD_CONNECTED BIT3
#define MESH_EVT_CHILD_DISCONNECTED BIT4
#define MESH_EVT_ROOT_GOT_IP BIT5

extern EventGroupHandle_t mesh_event_group;
extern bool mesh_connected;
extern bool mesh_is_root;
extern uint8_t parent_mac[6];
extern int mesh_layer;

void mesh_systems_init(void);
void mesh_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data);
