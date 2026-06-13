#ifndef HDC1080_H
#define HDC1080_H

#include "esp_err.h"

// Inicializa o barramento I2C
esp_err_t i2c_master_init(void);

// Realiza a leitura de temperatura e umidade do HDC1080
esp_err_t hdc1080_read_measurements(float *temperature, float *humidity);

#endif // HDC1080_H