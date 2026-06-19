#include "mqtt_task.h"
#include "config.h"
#include "sensor_task.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "MQTT_TASK";

#define MAX_READINGS 150

typedef struct {
    long timestamp;
    float temperature;
    float humidity;
} buffered_reading_t;

static void send_via_mqtt_task(void *pvParameters) {
    ESP_LOGI(TAG, "Tarefa sendViaMQTT iniciada.");
    
    while (1) {
        // Aguarda o intervalo de envio definido em config.h
        vTaskDelay(pdMS_TO_TICKS(MQTT_SEND_INTERVAL_MS));

        if (wifi_is_connected() && mqtt_is_connected()) {
            ESP_LOGI(TAG, "Conectado ao Wi-Fi e MQTT. Verificando dados locais...");

            buffered_reading_t *readings = malloc(sizeof(buffered_reading_t) * MAX_READINGS);
            if (readings == NULL) {
                ESP_LOGE(TAG, "Erro de alocacao de memoria para enviar leituras.");
                continue;
            }

            int count = 0;

            // Bloqueia acesso ao arquivo para leitura e limpeza concorrente
            if (xSemaphoreTake(file_mutex, portMAX_DELAY) == pdTRUE) {
                FILE *f = fopen(SPIFFS_DATA_FILE_PATH, "r");
                if (f != NULL) {
                    char line[64];
                    while (fgets(line, sizeof(line), f) != NULL && count < MAX_READINGS) {
                        long ts;
                        float t, h;
                        if (sscanf(line, "%ld,%f,%f", &ts, &t, &h) == 3) {
                            readings[count].timestamp = ts;
                            readings[count].temperature = t;
                            readings[count].humidity = h;
                            count++;
                        }
                    }
                    fclose(f);

                    // Limpa (wipe out) os dados locais ja lidos
                    FILE *f_clear = fopen(SPIFFS_DATA_FILE_PATH, "w");
                    if (f_clear != NULL) {
                        fclose(f_clear);
                        ESP_LOGI(TAG, "Dados locais transmitidos foram apagados com sucesso (%d leituras).", count);
                    } else {
                        ESP_LOGE(TAG, "Falha ao limpar o arquivo local.");
                    }
                } else {
                    ESP_LOGD(TAG, "Nenhum dado local para enviar.");
                }
                xSemaphoreGive(file_mutex);
            }

            // Publica os dados fora do semaforo para nao bloquear a tarefa de leitura do sensor
            for (int i = 0; i < count; i++) {
                char payload[128];
                snprintf(payload, sizeof(payload), "{\"temperature\": %.2f, \"timestamp\": %ld}", 
                         readings[i].temperature, readings[i].timestamp);
                
                ESP_LOGI(TAG, "Publicando no MQTT: %s", payload);
                mqtt_publish(MQTT_TOPIC_DATA, payload);
                
                // Delay curto entre mensagens para evitar congestionamento
                vTaskDelay(pdMS_TO_TICKS(100));
            }

            free(readings);
        } else {
            ESP_LOGW(TAG, "MQTT ou Wi-Fi desconectados. Os dados continuam retidos localmente.");
        }
    }
}

void mqtt_task_init(void) {
    xTaskCreate(send_via_mqtt_task, "mqtt_send_task", MQTT_TASK_STACK_SIZE, NULL, MQTT_TASK_PRIORITY, NULL);
}
