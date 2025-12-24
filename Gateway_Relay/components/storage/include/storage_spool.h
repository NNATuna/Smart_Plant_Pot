#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t node_id[8];
    uint32_t seq;
    size_t payload_len;
    uint8_t payload[256];
} spool_record_t;

typedef struct {
    uint32_t offset;
    size_t bytes_read;
    int records_read;
} spool_batch_info_t;

esp_err_t spool_init(void);
esp_err_t spool_append(const spool_record_t *record);
esp_err_t spool_read_batch(size_t max_records, size_t max_bytes, spool_record_t *out_records, spool_batch_info_t *info);
esp_err_t spool_commit(uint32_t new_offset);
esp_err_t spool_rotate_if_needed(size_t max_bytes);

#ifdef __cplusplus
}
#endif
