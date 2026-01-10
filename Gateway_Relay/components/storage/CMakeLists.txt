idf_component_register(
    SRCS "storage_spool.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_littlefs nvs_flash freertos
)
