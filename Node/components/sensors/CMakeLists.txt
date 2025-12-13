idf_component_register(
    SRCS 
        "sensors_manager.c"
        "bh1750.c"
        "aht20.c"
        "soil_sensor.c"
    INCLUDE_DIRS 
            "inc"
    REQUIRES 
        driver i2c_manager esp_adc
)
