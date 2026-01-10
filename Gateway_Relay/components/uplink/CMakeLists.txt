idf_component_register(
    SRCS "uplink.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_wifi esp_event esp_netif esp_timer freertos
)
