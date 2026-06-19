#include "sensor_task.h"
#include "config.h"
#include "hdc1080.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include <time.h>
#include <stdio.h>

static const char *TAG = "SENSOR_TASK";

// Variável estática (protegida) e seu respectivo Mutex
static sensor_data_t current_sensor_data = {0};
static SemaphoreHandle_t sensor_data_mutex = NULL;

// Mutex compartilhado para proteger o arquivo SPIFFS
SemaphoreHandle_t file_mutex = NULL;

/* * Documentação da Tarefa do FreeRTOS:
 * Nome: sensor_read_task
 * Propósito: Acordar a cada SENSOR_READ_INTERVAL_MS, fazer a leitura via I2C
 * e salvar os dados na estrutura de forma thread-safe usando Mutex.
 * Se o tempo estiver sincronizado, salva a leitura no armazenamento persistente.
 */
static void sensor_read_task(void *pvParameters) {
    float temp = 0.0;
    float hum = 0.0;

    while (1) {
        if (hdc1080_read_measurements(&temp, &hum) == ESP_OK) {
            ESP_LOGI(TAG, "Leitura Sucesso: Temp=%.2f C, Umid=%.2f %%", temp, hum);
            
            // Toma o mutex para escrever na região crítica em memória
            if (xSemaphoreTake(sensor_data_mutex, portMAX_DELAY) == pdTRUE) {
                current_sensor_data.temperature = temp;
                current_sensor_data.humidity = hum;
                current_sensor_data.is_valid = true;
                xSemaphoreGive(sensor_data_mutex);
            }

            // Tenta salvar os dados no SPIFFS com timestamp epoch se sincronizado
            time_t now;
            time(&now);
            if (now > 1000000000) { // Indica que o relogio sincronizou via SNTP (ano > 2001)
                if (xSemaphoreTake(file_mutex, portMAX_DELAY) == pdTRUE) {
                    FILE *f = fopen(SPIFFS_DATA_FILE_PATH, "a"); // Modo append
                    if (f != NULL) {
                        fprintf(f, "%ld,%.2f,%.2f\n", (long)now, temp, hum);
                        fclose(f);
                        ESP_LOGI(TAG, "Dado salvo localmente: Timestamp=%ld, Temp=%.2f, Umid=%.2f", (long)now, temp, hum);
                    } else {
                        ESP_LOGE(TAG, "Falha ao abrir arquivo SPIFFS para salvar leitura");
                    }
                    xSemaphoreGive(file_mutex);
                }
            } else {
                ESP_LOGW(TAG, "Hora do sistema ainda nao sincronizada. Ignorando gravacao local.");
            }

        } else {
            ESP_LOGE(TAG, "Falha ao ler o sensor");
            if (xSemaphoreTake(sensor_data_mutex, portMAX_DELAY) == pdTRUE) {
                current_sensor_data.is_valid = false;
                xSemaphoreGive(sensor_data_mutex);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));
    }
}

void sensor_task_init(void) {
    sensor_data_mutex = xSemaphoreCreateMutex();
    if (sensor_data_mutex == NULL) {
        ESP_LOGE(TAG, "Falha ao criar o Mutex do sensor");
        return;
    }

    file_mutex = xSemaphoreCreateMutex();
    if (file_mutex == NULL) {
        ESP_LOGE(TAG, "Falha ao criar o Mutex do arquivo local");
        return;
    }

    xTaskCreate(sensor_read_task, "sensor_task", SENSOR_TASK_STACK_SIZE, NULL, SENSOR_TASK_PRIORITY, NULL);
    ESP_LOGI(TAG, "Tarefa do Sensor inicializada.");
}

sensor_data_t sensor_get_latest_data(void) {
    sensor_data_t data_copy = {0};
    if (sensor_data_mutex != NULL && xSemaphoreTake(sensor_data_mutex, portMAX_DELAY) == pdTRUE) {
        data_copy = current_sensor_data;
        xSemaphoreGive(sensor_data_mutex);
    }
    return data_copy;
}