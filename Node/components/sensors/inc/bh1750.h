#ifndef BH1750_H
#define BH1750_H

#include "driver/i2c.h"
#include "esp_err.h"
#include "i2c_manager.h"

// BH1750 address
#define BH1750_ADDR_L 0x23
#define BH1750_ADDR_H 0x5C

typedef enum
{
    BH1750_CONT_H_RES_MODE = 0x10,  // 1 lx resolution, continuous
    BH1750_CONT_H_RES_MODE2 = 0x11, // 0.5 lx resolution
    BH1750_CONT_L_RES_MODE = 0x13,  // 4 lx resolution
    BH1750_ONE_H_RES_MODE = 0x20,   // 1 lx, one-shot
    BH1750_ONE_H_RES_MODE2 = 0x21,
    BH1750_ONE_L_RES_MODE = 0x23
} bh1750_mode_t;

typedef struct
{
    uint8_t addr;
    bh1750_mode_t mode;
} bh1750_t;

esp_err_t bh1750_init(bh1750_t *dev, uint8_t addr, bh1750_mode_t mode);
esp_err_t bh1750_read_lux(bh1750_t *dev, float *lux);

#endif
