#ifndef CONFIG_H
#define CONFIG_H

// Pinos I2C para o ESP32
#define I2C_MASTER_SCL_IO           19     
#define I2C_MASTER_SDA_IO           18     
#define I2C_MASTER_NUM              0       
#define I2C_MASTER_FREQ_HZ          100000  
#define I2C_MASTER_TX_BUF_DISABLE   0       
#define I2C_MASTER_RX_BUF_DISABLE   0       
#define I2C_MASTER_TIMEOUT_MS       1000

// Configurações do Sensor HDC1080
#define HDC1080_SENSOR_ADDR         0x40    

// Configurações da Tarefa do Sensor
#define SENSOR_TASK_STACK_SIZE      2048
#define SENSOR_TASK_PRIORITY        5
#define SENSOR_READ_INTERVAL_MS     2000

#endif // CONFIG_H
