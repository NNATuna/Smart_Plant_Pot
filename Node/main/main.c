#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sensors_manager.h"
#include "wifi_manager.h"

static const char *TAG = "APP_MAIN";
void app_main(void)
{
    wifi_init();
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    wifi_init_sta();
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    wifi_connect_start();
}
