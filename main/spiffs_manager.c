#include "spiffs_manager.h"
#include "config.h"
#include "esp_spiffs.h"
#include "esp_log.h"

static const char *TAG = "SPIFFS_MANAGER";

esp_err_t spiffs_manager_init(void) {
    ESP_LOGI(TAG, "Inicializando SPIFFS...");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = SPIFFS_BASE_PATH,          // Definido como "/spiffs" no config.h
        .partition_label = NULL,                // Usa a primeira partição do tipo spiffs encontrada
        .max_files = 5,                         // Número máximo de arquivos abertos concorrentemente
        .format_if_mount_failed = true          // Formata a partição caso ela não esteja formatada
    };

    // Registra o driver SPIFFS no Virtual File System (VFS) do ESP-IDF
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Falha ao montar ou formatar o filesystem SPIFFS.");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Falha ao encontrar a particao SPIFFS no partition table.");
        } else {
            ESP_LOGE(TAG, "Falha ao inicializar o SPIFFS (%s).", esp_err_to_name(ret));
        }
        return ret;
    }

    // Exibe o espaço total e ocupado na partição para depuração
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SPIFFS montado com sucesso. Total: %d bytes, Usado: %d bytes", total, used);
    } else {
        ESP_LOGE(TAG, "Falha ao obter informacoes da particao (%s).", esp_err_to_name(ret));
    }

    return ESP_OK;
}
