idf_component_register(
    SRCS "utils_common.c" "utils_metrics.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_timer freertos
)
