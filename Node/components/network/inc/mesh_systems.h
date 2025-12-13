#pragma once
#include "esp_mesh_lite.h"
#include "freertos/event_groups.h"

#define MESH_EVT_NODE_JOINED BIT0
#define MESH_EVT_NODE_LEFT BIT1
#define MESH_EVT_NODE_CHANGED BIT2

extern EventGroupHandle_t mesh_event_group;

extern bool mesh_connected;
extern uint8_t parent_mac[6];
extern int mesh_layer;

void mesh_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data);
