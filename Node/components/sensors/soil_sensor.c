#include "soil_sensor.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

static const char *TAG = "SOIL_SENSOR";

void soil_sensor_init(soil_sensor_config_t *cfg)
{
    esp_err_t ret;

    // -------------------------------
    // 1. Create ADC one-shot unit
    // -------------------------------
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = cfg->unit_id,
    };

    ret = adc_oneshot_new_unit(&init_cfg, &cfg->adc_handle);
    if (ret != ESP_OK || cfg->adc_handle == NULL)
    {
        ESP_LOGE(TAG, "adc_oneshot_new_unit FAILED: %s", esp_err_to_name(ret));
        return; // STOP HERE → prevent crash
    }

    ESP_LOGI(TAG, "ADC unit created: handle=%p", cfg->adc_handle);

    // -------------------------------
    // 2. Configure channel
    // -------------------------------
    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };

    ret = adc_oneshot_config_channel(cfg->adc_handle, cfg->channel, &chan_cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "adc_oneshot_config_channel FAILED: %s", esp_err_to_name(ret));
        return;
    }

    // -------------------------------
    // 3. Calibration (line fitting)
    // -------------------------------
    adc_cali_line_fitting_config_t cali_cfg = {
        .unit_id = cfg->unit_id,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    ret = adc_cali_create_scheme_line_fitting(&cali_cfg, &cfg->cali_handle);
    if (ret == ESP_OK)
    {
        cfg->calibrated = true;
        ESP_LOGI(TAG, "ADC calibration OK: cali_handle=%p", cfg->cali_handle);
    }
    else
    {
        cfg->calibrated = false;
        ESP_LOGW(TAG, "ADC calibration FAILED (ignored): %s", esp_err_to_name(ret));
    }

    ESP_LOGI(TAG, "Soil sensor initialized successfully");
}

uint32_t soil_sensor_read_raw(soil_sensor_config_t *cfg)
{
    if (!cfg->adc_handle) // Prevent crash
        return 0;

    int raw = 0;
    esp_err_t ret = adc_oneshot_read(cfg->adc_handle, cfg->channel, &raw);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "adc_oneshot_read FAILED: %s", esp_err_to_name(ret));
        return 0;
    }

    return raw;
}

uint32_t soil_sensor_read_voltage(soil_sensor_config_t *cfg)
{
    int raw = soil_sensor_read_raw(cfg);
    int mv = raw;

    if (cfg->calibrated)
    {
        esp_err_t ret = adc_cali_raw_to_voltage(cfg->cali_handle, raw, &mv);
        if (ret != ESP_OK)
            ESP_LOGW(TAG, "adc_cali_raw_to_voltage FAILED: %s", esp_err_to_name(ret));
    }

    return mv;
}

uint8_t soil_sensor_read_percent(soil_sensor_config_t *cfg)
{
    uint32_t raw = soil_sensor_read_raw(cfg);

    if (cfg->dry_value == cfg->wet_value)
        return 0;

    int percent = (cfg->dry_value - raw) * 100 /
                  (cfg->dry_value - cfg->wet_value);

    if (percent < 0)
        percent = 0;
    if (percent > 100)
        percent = 100;

    return percent;
}
