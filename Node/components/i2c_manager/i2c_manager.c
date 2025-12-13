#include "i2c_manager.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "I2C_MANAGER";

static SemaphoreHandle_t i2c_mutex = NULL;
static i2c_port_t i2c_port = I2C_NUM_0;
static uint32_t i2c_timeout = 100; // default 100ms

void i2c_manager_init(i2c_port_t port, uint32_t timeout_ms)
{
    i2c_port = port;
    i2c_timeout = timeout_ms;

    // Create mutex
    if (i2c_mutex == NULL)
        i2c_mutex = xSemaphoreCreateMutex();

    assert(i2c_mutex != NULL);

    // I2C config (GPIO 21/22 nếu bạn dùng board DevKit)
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    ESP_ERROR_CHECK(i2c_param_config(i2c_port, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0));

    ESP_LOGI(TAG, "I2C driver installed successfully on port %d", i2c_port);
}

esp_err_t i2c_manager_write(uint8_t addr, const uint8_t *data, size_t len)
{
    if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(i2c_timeout)) != pdTRUE)
        return ESP_ERR_TIMEOUT;

    esp_err_t ret = i2c_master_write_to_device(
        i2c_port,
        addr,
        data,
        len,
        pdMS_TO_TICKS(i2c_timeout));

    xSemaphoreGive(i2c_mutex);
    return ret;
}

esp_err_t i2c_manager_read(uint8_t addr, uint8_t *data, size_t len)
{
    if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(i2c_timeout)) != pdTRUE)
        return ESP_ERR_TIMEOUT;

    esp_err_t ret = i2c_master_read_from_device(
        i2c_port,
        addr,
        data,
        len,
        pdMS_TO_TICKS(i2c_timeout));

    xSemaphoreGive(i2c_mutex);
    return ret;
}

void i2c_manager_scan(i2c_port_t port)
{
    printf("🔍 Scanning I2C bus...\n");

    uint8_t dummy = 0x01; // POWER_ON command of BH1750

    for (uint8_t addr = 1; addr < 127; addr++)
    {
        esp_err_t ret = i2c_master_write_to_device(
            port,
            addr,
            &dummy,
            1,
            pdMS_TO_TICKS(10));

        if (ret == ESP_OK)
        {
            printf("✅ Found device at 0x%02X\n", addr);
        }
    }
    printf("🔍 Scan finished.\n");
}
