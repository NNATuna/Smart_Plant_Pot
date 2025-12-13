#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include "driver/i2c.h"
#include "freertos/semphr.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void i2c_manager_init(i2c_port_t port, uint32_t timeout_ms);

    esp_err_t i2c_manager_write(uint8_t addr, const uint8_t *data, size_t len);
    esp_err_t i2c_manager_read(uint8_t addr, uint8_t *data, size_t len);
    void i2c_manager_scan(i2c_port_t port);
#ifdef __cplusplus
}
#endif

#endif
