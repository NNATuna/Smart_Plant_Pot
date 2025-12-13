#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <stdint.h>
#include "soil_sensor.h"
#include "bh1750.h"
#include "aht20.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // Structure to hold all sensor readings
    typedef struct
    {
        float lux;            // Light intensity
        float temperature;    // AHT20 temperature
        float humidity;       // AHT20 humidity
        uint8_t soil_percent; // Soil moisture %
        uint32_t soil_raw;    // Raw ADC value
        uint32_t soil_mv;     // Voltage (mV)
    } sensor_data_t;

    // Sensor manager API
    void sensor_manager_init(void);
    void sensor_manager_read(sensor_data_t *data);

#ifdef __cplusplus
}
#endif

#endif // SENSOR_MANAGER_H
