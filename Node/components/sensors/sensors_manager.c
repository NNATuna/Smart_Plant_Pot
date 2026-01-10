#include "sensors_manager.h"
#include "bh1750.h"
#include "aht20.h"
//#include "soil_sensor.h"
#include "esp_log.h"

static const char *TAG = "SENSOR_MANAGER";

// --- Sensor instances ---
static bh1750_t light_sensor;
static aht20_t aht20_sensor;
static soil_sensor_config_t soil_cfg;

// --- Initialize all sensors ---
void sensor_manager_init(void)
{
    ESP_LOGI(TAG, "Initializing sensors...");

    // ---------------------------
    // BH1750 init
    // ---------------------------
    if (bh1750_init(&light_sensor, BH1750_ADDR_L, BH1750_CONT_H_RES_MODE) == ESP_OK)
        ESP_LOGI(TAG, "BH1750 initialized OK");
    else
        ESP_LOGE(TAG, "BH1750 init FAILED");

    // ---------------------------
    // AHT20 init
    // ---------------------------
    if (aht20_init(&aht20_sensor) == ESP_OK)
        ESP_LOGI(TAG, "AHT20 initialized OK");
    else
        ESP_LOGE(TAG, "AHT20 init FAILED");

    // ---------------------------
    // Soil Sensor init (ADC V2)
    // ---------------------------
    soil_cfg.unit_id = ADC_UNIT_1;
    soil_cfg.channel = ADC_CHANNEL_0; // GPIO36
    soil_cfg.dry_value = 3000;        // adjust to your soil
    soil_cfg.wet_value = 1200;

    soil_sensor_init(&soil_cfg);
    ESP_LOGI(TAG, "Soil sensor initialized");

    ESP_LOGI(TAG, "All sensors initialized.");
}

// --- Read all sensor values ---
void sensor_manager_read(sensor_data_t *data)
{
    if (!data)
        return;

    // Default fallback values
    data->lux = -1;
    data->temperature = -100;
    data->humidity = -1;
    data->soil_percent = 0;
    data->soil_raw = 0;
    data->soil_mv = 0;

    // ---------------------------
    // Read BH1750
    // ---------------------------
    if (bh1750_read_lux(&light_sensor, &data->lux) != ESP_OK)
        ESP_LOGW(TAG, "BH1750 read error");

    // ---------------------------
    // Read AHT20
    // ---------------------------
    if (aht20_read(&aht20_sensor, &data->temperature, &data->humidity) != ESP_OK)
        ESP_LOGW(TAG, "AHT20 read error");

    // ---------------------------
    // Read Soil Moisture Sensor (ADC V2)
    // ---------------------------
    data->soil_raw = soil_sensor_read_raw(&soil_cfg);
    data->soil_mv = soil_sensor_read_voltage(&soil_cfg);
    data->soil_percent = soil_sensor_read_percent(&soil_cfg);
}
