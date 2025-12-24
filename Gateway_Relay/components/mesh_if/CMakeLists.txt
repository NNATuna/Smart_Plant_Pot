idf_component_register(
    SRCS "mesh_if.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_wifi esp_event esp_netif esp_mesh freertos
)
