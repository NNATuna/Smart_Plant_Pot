idf_component_register(
    SRCS "pipeline.c"
    INCLUDE_DIRS "include"
    REQUIRES freertos json mesh_if storage registry utils
)
