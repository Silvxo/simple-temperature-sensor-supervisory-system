#include "esp_log.h"
#include "hdc1080.h"
#include "sensor_task.h"

static const char *TAG = "APP_MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Iniciando Estacao Meteorologica...");

    // Inicializa o barramento I2C
    if (i2c_master_init() == ESP_OK) {
        ESP_LOGI(TAG, "I2C Inicializado com sucesso.");
    } else {
        ESP_LOGE(TAG, "Falha ao inicializar o I2C.");
        return; // Sem I2C o sistema não pode prosseguir
    }

    // Inicializa a task do sensor e o Mutex (RTOS)
    sensor_task_init();

    // O main.c encerra aqui, as tasks do FreeRTOS assumem o controle.
}
