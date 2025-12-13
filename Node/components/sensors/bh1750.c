#include "i2c_manager.h"
#include "bh1750.h"
#include "esp_log.h"
#include "freertos/task.h"

static const char *TAG = "BH1750";

static esp_err_t bh1750_cmd(bh1750_t *dev, uint8_t cmd)
{
    return i2c_manager_write(dev->addr, &cmd, 1);
}

esp_err_t bh1750_power_on(bh1750_t *dev)
{
    return bh1750_cmd(dev, 0x01);
}
esp_err_t bh1750_power_off(bh1750_t *dev)
{
    return bh1750_cmd(dev, 0x00);
}
esp_err_t bh1750_reset(bh1750_t *dev)
{
    return bh1750_cmd(dev, 0x07);
}
esp_err_t bh1750_init(bh1750_t *dev, uint8_t addr, bh1750_mode_t mode)
{
    if (!dev)
        return ESP_ERR_INVALID_ARG;

    dev->addr = addr;
    dev->mode = mode;

    esp_err_t ret;

    // 1) Power ON
    ret = bh1750_power_on(dev);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Power ON failed");
        return ret;
    }
    // 2) Reset
    ret = bh1750_reset(dev);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Reset failed");
        return ret;
    }

    // 3) Set measurement mode
    ret = bh1750_cmd(dev, mode);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Set mode failed");
        return ret;
    }

    ESP_LOGI(TAG, "BH1750 initialized at 0x%02X", addr);
    return ESP_OK;
}

esp_err_t bh1750_read_lux(bh1750_t *dev, float *lux)
{
    if (!dev || !lux)
        return ESP_ERR_INVALID_ARG;

    uint8_t data[2];

    // Chờ cảm biến đo (theo datasheet 120–180 ms)
    vTaskDelay(pdMS_TO_TICKS(180));

    esp_err_t ret = i2c_manager_read(dev->addr, data, 2);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Read failed");
        return ret;
    }

    uint16_t raw = (data[0] << 8) | data[1];
    *lux = raw / 1.2f;

    return ESP_OK;
}
