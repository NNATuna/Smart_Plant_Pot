#include "i2c_manager.h"
#include "aht20.h"
#include "esp_log.h"
#include "freertos/task.h"

static const char *TAG = "AHT20";

// -----------------------
// Send cmd to AHT20
// -----------------------
static esp_err_t aht20_cmd(aht20_t *dev, const uint8_t *cmd, size_t len)
{
    return i2c_manager_write(dev->addr, cmd, len);
}

// -----------------------
// Init AHT20
// -----------------------
esp_err_t aht20_init(aht20_t *dev)
{
    if (!dev)
        return ESP_ERR_INVALID_ARG;

    dev->addr = AHT20_ADDR;

    uint8_t init_cmd[] = {0xBE, 0x08, 0x00}; // init command

    ESP_LOGI(TAG, "Initializing AHT20...");

    esp_err_t ret = aht20_cmd(dev, init_cmd, sizeof(init_cmd));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Init command failed");
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(20)); // datasheet

    ESP_LOGI(TAG, "AHT20 init OK");

    return ESP_OK;
}

// -----------------------
// Read temperature and humidity
// -----------------------
esp_err_t aht20_read(aht20_t *dev, float *temperature, float *humidity)
{
    if (!dev || !temperature || !humidity)
        return ESP_ERR_INVALID_ARG;

    uint8_t measure_cmd[] = {0xAC, 0x33, 0x00};

    // Gửi lệnh đo
    esp_err_t ret = aht20_cmd(dev, measure_cmd, sizeof(measure_cmd));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Measure command failed");
        return ret;
    }

    // AHT20 need 80ms to measure
    vTaskDelay(pdMS_TO_TICKS(80));

    // Read 7 bytes data
    uint8_t data[7];
    ret = i2c_manager_read(dev->addr, data, 7);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Read failed");
        return ret;
    }

    if (data[0] & 0x80)
    {
        ESP_LOGW(TAG, "Sensor is busy");
        return ESP_ERR_INVALID_STATE;
    }

    // Parse humidity 20-bit
    uint32_t h_raw = ((uint32_t)data[1] << 12) |
                     ((uint32_t)data[2] << 4) |
                     ((uint32_t)data[3] >> 4);

    // Parse temperature 20-bit
    uint32_t t_raw = (((uint32_t)data[3] & 0x0F) << 16) |
                     ((uint32_t)data[4] << 8) |
                     (uint32_t)data[5];

    // Convert
    *humidity = (h_raw * 100.0f) / 1048576.0f;
    *temperature = ((t_raw * 200.0f) / 1048576.0f) - 50.0f;

    return ESP_OK;
}
