#pragma once

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WIFI_STA_DISCONNECTED = 0,
    WIFI_STA_CONNECTING,
    WIFI_STA_CONNECTED,
    WIFI_STA_GOT_IP,
} wifi_sta_state_t;

esp_err_t wifi_sta_init(const char *ssid, const char *password);
wifi_sta_state_t wifi_sta_get_state(void);
esp_err_t time_sync_init(void);

#ifdef __cplusplus
}
#endif
