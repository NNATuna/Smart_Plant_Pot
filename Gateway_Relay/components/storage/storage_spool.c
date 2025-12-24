#include "storage_spool.h"
#include "esp_log.h"
#include "esp_littlefs.h"
#include "esp_vfs.h"
#include "sdkconfig.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "spool";
static uint32_t s_commit_offset = 0;

static esp_err_t mount_littlefs(void)
{
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/spool",
        .partition_label = CONFIG_SPOOL_PARTITION_LABEL,
        .format_if_mount_failed = true,
        .dont_mount = false,
    };
    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount LittleFS (%s)", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t spool_init(void)
{
    return mount_littlefs();
}

esp_err_t spool_append(const spool_record_t *record)
{
    if (!record) {
        return ESP_ERR_INVALID_ARG;
    }
    FILE *f = fopen("/spool/spool.log", "ab");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open spool file");
        return ESP_FAIL;
    }
    fwrite(record, sizeof(spool_record_t), 1, f);
    fflush(f);
    fclose(f);
    return ESP_OK;
}

esp_err_t spool_read_batch(size_t max_records, size_t max_bytes, spool_record_t *out_records, spool_batch_info_t *info)
{
    if (!out_records || !info) {
        return ESP_ERR_INVALID_ARG;
    }
    FILE *f = fopen("/spool/spool.log", "rb");
    if (!f) {
        return ESP_FAIL;
    }
    fseek(f, info->offset, SEEK_SET);
    size_t bytes = 0;
    int count = 0;
    while (count < max_records && bytes + sizeof(spool_record_t) <= max_bytes) {
        if (fread(&out_records[count], sizeof(spool_record_t), 1, f) != 1) {
            break;
        }
        bytes += sizeof(spool_record_t);
        count++;
    }
    info->bytes_read = bytes;
    info->records_read = count;
    info->offset += bytes;
    fclose(f);
    return ESP_OK;
}

esp_err_t spool_commit(uint32_t new_offset)
{
    s_commit_offset = new_offset;
    return ESP_OK;
}

esp_err_t spool_rotate_if_needed(size_t max_bytes)
{
    FILE *f = fopen("/spool/spool.log", "rb");
    if (!f) {
        return ESP_OK;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    if (size > (long)max_bytes) {
        ESP_LOGI(TAG, "Rotating spool, size=%ld", size);
        remove("/spool/spool.log");
    }
    return ESP_OK;
}
