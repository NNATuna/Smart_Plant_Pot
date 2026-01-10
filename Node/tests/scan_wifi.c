#include "esp_wifi.h"
#include "esp_log.h"

static const char *TAG = "WIFI_SCAN";

void wifi_scan_and_print(void)
{
    wifi_scan_config_t scan_cfg = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0, // scan ALL channels
        .show_hidden = true};

    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_cfg, true));

    uint16_t ap_num = 20;
    wifi_ap_record_t ap_records[20];
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));

    ESP_LOGI(TAG, "Found %d APs", ap_num);

    for (int i = 0; i < ap_num; i++)
    {
        ESP_LOGI(TAG,
                 "SSID:%s | CH:%d | RSSI:%d | AUTH:%d",
                 ap_records[i].ssid,
                 ap_records[i].primary,
                 ap_records[i].rssi,
                 ap_records[i].authmode);
    }
}
