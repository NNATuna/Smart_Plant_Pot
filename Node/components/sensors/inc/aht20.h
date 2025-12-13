#ifndef AHT20_H
#define AHT20_H

#include "driver/i2c.h"
#include "esp_err.h"
#include "i2c_manager.h"

#define AHT20_ADDR  0x38

typedef struct {
    uint8_t addr;
} aht20_t;

// API chính
esp_err_t aht20_init(aht20_t *dev);
esp_err_t aht20_read(aht20_t *dev, float *temperature, float *humidity);

#endif
