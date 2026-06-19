# Weather Station — ESP32 + HDC1080

Projeto da disciplina de **Sistemas Embarcados** — Engenharia da Computação.

---

## Visão Geral

Estação meteorológica embarcada com leitura de temperatura e umidade via sensor **HDC1080**,
executando sobre **FreeRTOS** no ESP32 com **ESP-IDF** (sem abstração Arduino) com armazenamento persistente em flash, sincronização temporal SNTP, envio via MQTT e Dashboard Flask para monitoramento histórico filtrado.

---

## Arquitetura do Projeto

```
weather_station/
├── CMakeLists.txt               # Raiz do projeto ESP-IDF
├── partitions.csv               # Tabela de partições do ESP32 (customizada)
├── sdkconfig.defaults           # Configurações padrão do SDK
│
├── main/
│   ├── CMakeLists.txt           # Configuração do componente main
│   ├── idf_component.yml        # Manifesto de dependências (esp-mqtt)
│   ├── config.h                 # ★ Constantes globais (pinos, wifi, mqtt)
│   ├── main.c                   # Ponto de entrada — orquestração de inits
│   ├── hdc1080.c / .h           # Driver de hardware: comunicação I2C com o HDC1080
│   ├── sensor_task.c / .h       # Leitura do sensor + escrita local em SPIFFS (thread-safe)
│   ├── wifi_manager.c / .h      # Conexão Wi-Fi (modo STA) + sincronização de tempo SNTP
│   ├── mqtt_manager.c / .h      # Driver/Wrapper de conectividade MQTT
│   ├── mqtt_task.c / .h         # Leitura local do arquivo e transmissão via MQTT
│   └── spiffs_manager.c / .h    # Montagem e inicialização do sistema de arquivos SPIFFS
│
└── dashboard/                   # Sistema de supervisão (Flask)
    ├── app.py                   # Servidor Flask com receptor MQTT
    ├── models.py                # Modelo de banco de dados SQLAlchemy (SQLite)
    ├── mock_sensor.py           # Simulador de sensor MQTT
    ├── requirements.txt         # Dependências do Python
    └── templates/
        └── index.html           # Interface web com gráfico e dropdown de período
```

### Diagrama de camadas

```
┌─────────────────────────────────┐
│           main.c                │  ← Orquestração apenas
├─────────────────────────────────┤
│    sensor_task / mqtt_task      │  ← Lógica: tarefas, mutex de arquivos
├─────────────────────────────────┤
│  wifi / mqtt / spiffs managers  │  ← Abstração de drivers de rede/arquivos
├─────────────────────────────────┤
│           hdc1080               │  ← Driver: I2C, registradores
├─────────────────────────────────┤
│      ESP-IDF (HAL, VFS, lwIP)   │  ← HAL do fabricante
└─────────────────────────────────┘
```

---

## Hardware

| Sinal     | ESP32 GPIO | HDC1080 |
|-----------|-----------|---------|
| SDA       | GPIO 18   | SDA     |
| SCL       | GPIO 19   | SCL     |
| Alimentação | 3.3V   | VCC     |
| GND       | GND       | GND     |

> Os pinos SDA e SCL podem ser alterados em `main/config.h`.

---

## Requisitos Atendidos

| Requisito                        | Implementação                                      |
|----------------------------------|----------------------------------------------------|
| ESP-IDF sem Arduino              | ✅ ESP-IDF nativo, `driver/i2c.h`                  |
| RTOS                             | ✅ FreeRTOS (incluso no ESP-IDF)                   |
| Tarefas paralelas                | ✅ `sensor_task` + `mqtt_send_task`                |
| Proteção de memória crítica      | ✅ Mutex `sensor_data_mutex` em `s_sensor_data`    |
| Interfaceamento com sensor       | ✅ HDC1080 via I2C (temperatura e umidade)         |
| Constantes em único local        | ✅ `main/config.h`                                 |
| Logs de depuração                | ✅ `ESP_LOGI/LOGE/LOGD` em todos os módulos        |
| Conectividade Wi-Fi              | ✅ `wifi_manager` com reconexão automática          |
| Conectividade MQTT               | ✅ `mqtt_manager` + `mqtt_task` publicando dados    |
| Armazenamento local persistente  | ✅ Salvamento em flash (SPIFFS) c/ timestamp Epoch |
| Sincronização de relógio         | ✅ SNTP para obtenção do tempo real (Epoch)        |
| Exclusão de arquivos segura      | ✅ Semáforo `file_mutex` para acesso ao SPIFFS     |
| Dashboard Web c/ Filtro          | ✅ Flask c/ filtro de período (15m a 24h)          |

---

## Como Compilar

```bash
# Configure o target
idf.py set-target esp32

# Compile (O IDF Component Manager baixará automaticamente a dependência MQTT)
idf.py build

# Grave e monitore
idf.py -p /dev/ttyUSB0 flash monitor
```
