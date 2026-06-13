#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include <stdbool.h>

// Estrutura encapsulada para armazenar as leituras
typedef struct {
    float temperature;
    float humidity;
    bool is_valid; // Indica se a última leitura foi bem sucedida
} sensor_data_t;

// Inicializa a task e os mecanismos de sincronização (Mutex)
void sensor_task_init(void);

// Recupera a última leitura de forma segura (Thread-Safe)
sensor_data_t sensor_get_latest_data(void);

#endif // SENSOR_TASK_H