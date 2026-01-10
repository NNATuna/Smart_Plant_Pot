#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sensors_manager.h"
#include "mesh_manager.h"
#include "scan_wifi.h"

static const char *TAG = "APP_MAIN";
void app_main(void)
{
    // mesh_manager_init();
    // wifi_scan_and_print();
    // mesh_manager_start();

    i2c_manager_init(I2C_NUM_0, 100);
    ESP_LOGI(TAG, "Sensor test start");
    sensor_manager_init();
    sensor_data_t data;
    int t = 5;
    while (t--)
    {
        sensor_manager_read(&data);

        ESP_LOGI(TAG,
                 "LUX: %.2f lx | TEMP: %.2f C | HUM: %.2f %% | SOIL: %d %% (raw=%d, %d mV)",
                 data.lux,
                 data.temperature,
                 data.humidity,
                 data.soil_percent,
                 data.soil_raw,
                 data.soil_mv);

        vTaskDelay(pdMS_TO_TICKS(2000)); // đọc mỗi 2 giây
    }
}
