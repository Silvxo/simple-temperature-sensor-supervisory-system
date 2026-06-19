#ifndef SPIFFS_MANAGER_H
#define SPIFFS_MANAGER_H

#include "esp_err.h"

/**
 * @brief Inicializa e monta a partição SPIFFS para armazenamento local persistente.
 * 
 * @return esp_err_t ESP_OK em caso de sucesso.
 */
esp_err_t spiffs_manager_init(void);

#endif // SPIFFS_MANAGER_H
