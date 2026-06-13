/**
 * @file main.c
 * @brief Ponto de entrada da aplicação Weather Station.
 *
 * O main.c tem responsabilidade única: orquestrar a inicialização
 * dos módulos na ordem correta e não conter lógica de negócio.
 *
 * Fluxo de inicialização:
 *   app_main()
 *     └── sensor_manager_init()
 *           ├── hdc1080_init()       (driver I2C)
 *           └── xTaskCreate()        (sensor_task - FreeRTOS)
 *
 * Módulos planejados para próximas etapas:
 *   - wifi_manager_init()    (conectividade Wi-Fi)
 *   - mqtt_manager_init()    (publicação de dados via MQTT)
 *   - http_server_init()     (dashboard web local)
 */

#include "sensor_manager.h"
#include "config.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief Ponto de entrada principal do firmware.
 *
 * Chamado pelo ESP-IDF após inicialização do hardware.
 * Deve permanecer simples: apenas inicializações e, se necessário,
 * uma tarefa de supervisão de alto nível.
 */
void app_main(void)
{
    ESP_LOGI(CONFIG_LOG_TAG_MAIN, "=== Weather Station - Iniciando ===");
    ESP_LOGI(CONFIG_LOG_TAG_MAIN, "IDF versão: %s", esp_get_idf_version());

    /* Inicializa o gerenciador de sensores (cria tarefa FreeRTOS internamente) */
    esp_err_t ret = sensor_manager_init();
    if (ret != ESP_OK) {
        ESP_LOGE(CONFIG_LOG_TAG_MAIN,
                 "Falha crítica ao inicializar sensor_manager: %s. Sistema parado.",
                 esp_err_to_name(ret));
        /* Em um sistema embarcado real, considere reiniciar: esp_restart() */
        return;
    }

    ESP_LOGI(CONFIG_LOG_TAG_MAIN, "Sistema inicializado. Tarefas em execução.");

    /*
     * O app_main() pode retornar normalmente no ESP-IDF.
     * As tarefas FreeRTOS criadas continuarão executando no scheduler.
     *
     * Aqui poderíamos inicializar outros módulos futuramente:
     *   wifi_manager_init();
     *   mqtt_manager_init();
     *   http_server_init();
     */
}
