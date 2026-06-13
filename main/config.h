/**
 * @file config.h
 * @brief Constantes globais do projeto Weather Station.
 *
 * Ponto único de definição de todas as constantes de configuração.
 * Nenhum outro módulo deve definir constantes de hardware ou sistema aqui duplicadas.
 */

#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================
 * Configurações de Hardware - I2C
 * ========================================================= */

/** @brief Pino SDA do barramento I2C */
#define CONFIG_I2C_SDA_PIN          21

/** @brief Pino SCL do barramento I2C */
#define CONFIG_I2C_SCL_PIN          22

/** @brief Frequência do clock I2C em Hz (400kHz = Fast Mode) */
#define CONFIG_I2C_FREQ_HZ          400000

/** @brief Porta I2C utilizada pelo projeto */
#define CONFIG_I2C_PORT             I2C_NUM_0

/* =========================================================
 * Configurações do Sensor HDC1080
 * ========================================================= */

/** @brief Endereço I2C do HDC1080 (fixo em hardware) */
#define CONFIG_HDC1080_I2C_ADDR     0x40

/** @brief Tempo de aquecimento do sensor após power-on em ms */
#define CONFIG_HDC1080_WARMUP_MS    15

/* =========================================================
 * Configurações das Tarefas FreeRTOS
 * ========================================================= */

/** @brief Período de leitura do sensor em ms */
#define CONFIG_SENSOR_TASK_PERIOD_MS        2000

/** @brief Tamanho da stack da tarefa de leitura do sensor (words) */
#define CONFIG_SENSOR_TASK_STACK_SIZE       4096

/** @brief Prioridade da tarefa de leitura do sensor */
#define CONFIG_SENSOR_TASK_PRIORITY         5

/** @brief Nome identificador da tarefa de leitura do sensor */
#define CONFIG_SENSOR_TASK_NAME             "sensor_task"

/* =========================================================
 * Configurações de Logging
 * ========================================================= */

/** @brief Tag de log para o módulo principal */
#define CONFIG_LOG_TAG_MAIN         "MAIN"

/** @brief Tag de log para o driver HDC1080 */
#define CONFIG_LOG_TAG_HDC1080      "HDC1080"

/** @brief Tag de log para o gerenciador de sensores */
#define CONFIG_LOG_TAG_SENSOR_MGR   "SENSOR_MGR"

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
