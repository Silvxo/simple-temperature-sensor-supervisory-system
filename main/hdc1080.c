#include "hdc1080.h"
#include "config.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "HDC1080_DRIVER";

esp_err_t i2c_master_init(void) {
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

esp_err_t hdc1080_read_measurements(float *temperature, float *humidity) {
    uint8_t reg_addr = 0x00; // Registrador de temperatura (sequencial para umidade)
    uint8_t data[4];
    
    // Dispara a conversão escrevendo no registrador 0x00
    esp_err_t err = i2c_master_write_to_device(I2C_MASTER_NUM, HDC1080_SENSOR_ADDR, &reg_addr, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao iniciar conversão I2C");
        return err;
    }

    // O HDC1080 precisa de cerca de 15ms para realizar a conversão (14-bits)
    vTaskDelay(pdMS_TO_TICKS(20));

    // Lê os 4 bytes (2 de Temp, 2 de Umidade)
    err = i2c_master_read_from_device(I2C_MASTER_NUM, HDC1080_SENSOR_ADDR, data, 4, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erro na leitura dos dados I2C");
        return err;
    }

    uint16_t raw_temp = (data[0] << 8) | data[1];
    uint16_t raw_hum = (data[2] << 8) | data[3];

    // Fórmulas do datasheet do HDC1080
    *temperature = ((raw_temp / 65536.0) * 165.0) - 40.0;
    *humidity = (raw_hum / 65536.0) * 100.0;

    return ESP_OK;
}