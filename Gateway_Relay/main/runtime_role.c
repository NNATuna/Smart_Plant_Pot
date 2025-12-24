#include "runtime_role.h"
#include "mesh_if.h"
#include "esp_log.h"

static const char *TAG = "runtime_role";
static runtime_role_t s_role = ROLE_RELAY;
static bool s_can_uplink = true;

esp_err_t runtime_role_init(bool can_uplink)
{
    s_can_uplink = can_uplink;
    s_role = can_uplink ? ROLE_GATEWAY_ROOT : ROLE_RELAY;
    ESP_LOGI(TAG, "Runtime role initialized: %s", s_role == ROLE_GATEWAY_ROOT ? "root" : "relay");
    return ESP_OK;
}

runtime_role_t runtime_role_get(void)
{
    return s_role;
}

bool runtime_role_can_uplink(void)
{
    return s_can_uplink && s_role == ROLE_GATEWAY_ROOT;
}

void runtime_role_on_mesh_event(int event_id)
{
    if (!s_can_uplink) {
        s_role = ROLE_RELAY;
        return;
    }
    if (event_id == MESH_EVENT_ROOT_GOT_IP) {
        s_role = ROLE_GATEWAY_ROOT;
    } else if (event_id == MESH_EVENT_ROOT_LOST_IP) {
        s_role = ROLE_RELAY;
    }
    ESP_LOGI(TAG, "Role changed to %d", s_role);
}
