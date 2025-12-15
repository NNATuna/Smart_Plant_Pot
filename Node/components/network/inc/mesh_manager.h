#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t mesh_manager_init(void);
esp_err_t mesh_manager_start(void);
esp_err_t mesh_manager_stop(void);
bool mesh_manager_is_root(void);

#ifdef __cplusplus
}
#endif

#endif // MESH_MANAGER_H
