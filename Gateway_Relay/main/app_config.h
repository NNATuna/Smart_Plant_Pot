#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char wifi_ssid[32];
    char wifi_password[64];
    char cloud_base_url[128];
    char cloud_auth_token[128];
    int batch_time_sec;
    int batch_max_records;
} app_config_t;

esp_err_t app_config_init(app_config_t *config);
const app_config_t *app_config_get(void);

#ifdef __cplusplus
}
#endif
