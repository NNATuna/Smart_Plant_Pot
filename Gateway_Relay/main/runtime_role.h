#pragma once

#include <stdbool.h>
#include "esp_err.h"

typedef enum {
    ROLE_OFFLINE = 0,
    ROLE_RELAY,
    ROLE_GATEWAY_ROOT,
} runtime_role_t;

esp_err_t runtime_role_init(bool can_uplink);
runtime_role_t runtime_role_get(void);
bool runtime_role_can_uplink(void);
void runtime_role_on_mesh_event(int event_id);
