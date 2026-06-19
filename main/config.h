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

// Configurações de Wi-Fi
#define WIFI_SSID                   "Sangsunga"
#define WIFI_PASS                   "ibis2211"
#define WIFI_MAXIMUM_RETRY          5

// Configurações de MQTT
#define MQTT_BROKER_URL             "mqtt://broker.hivemq.com:1883"
#define MQTT_TOPIC_DATA             "silve/temp-supervisory-sys/esp32-sensor/data"
#define MQTT_SEND_INTERVAL_MS       10000   // Envia dados a cada 10 segundos
#define MQTT_TASK_STACK_SIZE        4096
#define MQTT_TASK_PRIORITY          4

// Armazenamento Persistente (SPIFFS)
#define SPIFFS_BASE_PATH            "/spiffs"
#define SPIFFS_DATA_FILE_PATH       "/spiffs/readings.csv"

#endif // CONFIG_H

