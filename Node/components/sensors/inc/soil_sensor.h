#ifndef SOIL_SENSOR_H
#define SOIL_SENSOR_H

#include <stdint.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

typedef struct
{
    adc_unit_t unit_id;
    adc_channel_t channel;
    uint32_t dry_value;
    uint32_t wet_value;

    adc_oneshot_unit_handle_t adc_handle;
    adc_cali_handle_t cali_handle;
    bool calibrated;

} soil_sensor_config_t;

void soil_sensor_init(soil_sensor_config_t *cfg);

uint32_t soil_sensor_read_raw(soil_sensor_config_t *cfg);
uint32_t soil_sensor_read_voltage(soil_sensor_config_t *cfg);
uint8_t soil_sensor_read_percent(soil_sensor_config_t *cfg);

#endif
