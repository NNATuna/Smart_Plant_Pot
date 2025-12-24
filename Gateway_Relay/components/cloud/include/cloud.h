#pragma once

#include "esp_err.h"
#include "storage_spool.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t cloud_client_init(const char *base_url, const char *auth_token);
esp_err_t cloud_sync_start(void);
esp_err_t cloud_config_pull_start(void);

#ifdef __cplusplus
}
#endif
