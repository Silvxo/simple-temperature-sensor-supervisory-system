#include "esp_log.h"
#include "nvs_flash.h"
#include "hdc1080.h"
#include "sensor_task.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "spiffs_manager.h"
#include "mqtt_task.h"

static const char *TAG = "APP_MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Iniciando Estacao Meteorologica...");

    // 1. Inicializa o NVS (necessário para o driver Wi-Fi gravar configurações)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Inicializa o barramento I2C
    if (i2c_master_init() == ESP_OK) {
        ESP_LOGI(TAG, "I2C Inicializado com sucesso.");
    } else {
        ESP_LOGE(TAG, "Falha ao inicializar o I2C.");
        return; // Sem I2C o sistema não pode prosseguir
    }

    // 3. Monta o sistema de arquivos SPIFFS
    if (spiffs_manager_init() != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao montar o SPIFFS. O armazenamento local nao funcionara.");
    }

    // 4. Inicializa o Wi-Fi (modo station) e sincronização SNTP
    wifi_init_sta();

    // 5. Inicia o cliente MQTT
    mqtt_app_start();

    // 6. Inicializa as tarefas do FreeRTOS
    sensor_task_init(); // Tarefa de leitura do sensor e salvamento local
    mqtt_task_init();   // Tarefa de leitura de log e envio via MQTT

    ESP_LOGI(TAG, "Todos os subsistemas inicializados. FreeRTOS controlando tarefas.");
}

