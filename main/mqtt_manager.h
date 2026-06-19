#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <stdbool.h>
#include "esp_err.h"

/**
 * @brief Inicializa o cliente MQTT com as configuracoes do config.h.
 */
void mqtt_app_start(void);

/**
 * @brief Retorna se o cliente MQTT esta conectado ao Broker.
 */
bool mqtt_is_connected(void);

/**
 * @brief Publica uma mensagem em um determinado topico.
 * 
 * @param topic Topico MQTT.
 * @param data Mensagem/Payload.
 * @return esp_err_t ESP_OK em caso de sucesso, ESP_FAIL caso contrario.
 */
esp_err_t mqtt_publish(const char *topic, const char *data);

#endif // MQTT_MANAGER_H
