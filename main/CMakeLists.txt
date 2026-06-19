idf_component_register(SRCS "sensor_task.c" "hdc1080.c" "main.c" "wifi_manager.c" "mqtt_manager.c" "spiffs_manager.c" "mqtt_task.c"
                    INCLUDE_DIRS "."
                    REQUIRES mqtt esp_wifi nvs_flash spiffs driver esp_event)



