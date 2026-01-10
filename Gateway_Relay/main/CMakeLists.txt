idf_component_register(
    SRCS "app_main.c" "runtime_role.c" "app_config.c"
    INCLUDE_DIRS "."
    REQUIRES mesh_if pipeline registry storage uplink cloud bridge utils
)
