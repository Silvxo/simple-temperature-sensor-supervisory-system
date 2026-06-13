#include "sensor_task.h"
#include "config.h"
#include "hdc1080.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "SENSOR_TASK";

// Variável estática (protegida) e seu respectivo Mutex
static sensor_data_t current_sensor_data = {0};
static SemaphoreHandle_t sensor_data_mutex = NULL;

/* * Documentação da Tarefa do FreeRTOS:
 * Nome: sensor_read_task
 * Propósito: Acordar a cada SENSOR_READ_INTERVAL_MS, fazer a leitura via I2C
 * e salvar os dados na estrutura de forma thread-safe usando Mutex.
 */
static void sensor_read_task(void *pvParameters) {
    float temp = 0.0;
    float hum = 0.0;

    while (1) {
        if (hdc1080_read_measurements(&temp, &hum) == ESP_OK) {
            ESP_LOGI(TAG, "Leitura Sucesso: Temp=%.2f C, Umid=%.2f %%", temp, hum);
            
            // Toma o mutex para escrever na região crítica
            if (xSemaphoreTake(sensor_data_mutex, portMAX_DELAY) == pdTRUE) {
                current_sensor_data.temperature = temp;
                current_sensor_data.humidity = hum;
                current_sensor_data.is_valid = true;
                xSemaphoreGive(sensor_data_mutex); // Libera o mutex
            }
        } else {
            ESP_LOGE(TAG, "Falha ao ler o sensor");
            if (xSemaphoreTake(sensor_data_mutex, portMAX_DELAY) == pdTRUE) {
                current_sensor_data.is_valid = false;
                xSemaphoreGive(sensor_data_mutex);
            }
        }

        // Bloqueia a tarefa até o próximo ciclo
        vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));
    }
}

void sensor_task_init(void) {
    // Criação do Mutex para proteção da memória crítica
    sensor_data_mutex = xSemaphoreCreateMutex();
    if (sensor_data_mutex == NULL) {
        ESP_LOGE(TAG, "Falha ao criar o Mutex do sensor");
        return;
    }

    // Cria a tarefa de leitura
    xTaskCreate(sensor_read_task, "sensor_task", SENSOR_TASK_STACK_SIZE, NULL, SENSOR_TASK_PRIORITY, NULL);
    ESP_LOGI(TAG, "Tarefa do Sensor inicializada.");
}

sensor_data_t sensor_get_latest_data(void) {
    sensor_data_t data_copy = {0};
    
    // Toma o mutex para leitura da região crítica (Thread-Safe Getter)
    if (sensor_data_mutex != NULL && xSemaphoreTake(sensor_data_mutex, portMAX_DELAY) == pdTRUE) {
        data_copy = current_sensor_data;
        xSemaphoreGive(sensor_data_mutex);
    }
    
    return data_copy;
}