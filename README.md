# Weather Station — ESP32 + HDC1080

Projeto da disciplina de **Sistemas Embarcados** — Engenharia da Computação.

---

## Visão Geral

Estação meteorológica embarcada com leitura de temperatura e umidade via sensor **HDC1080**,
executando sobre **FreeRTOS** no ESP32 com **ESP-IDF** (sem abstração Arduino).

---

## Arquitetura do Projeto

```
weather_station/
├── CMakeLists.txt               # Raiz do projeto ESP-IDF
├── sdkconfig.defaults           # Configurações padrão do SDK
│
├── main/
│   ├── CMakeLists.txt
│   ├── config.h                 # ★ Constantes globais (único local)
│   └── main.c                   # Ponto de entrada — apenas orquestra inits
│
└── components/
    ├── hdc1080/                 # Camada: Driver de hardware
    │   ├── CMakeLists.txt
    │   ├── include/
    │   │   └── hdc1080.h        # API pública do driver
    │   └── hdc1080.c            # Comunicação I2C com o sensor
    │
    └── sensor_manager/          # Camada: Lógica de aplicação
        ├── CMakeLists.txt
        ├── include/
        │   └── sensor_manager.h # API pública + tipos de dados
        └── sensor_manager.c     # Tarefa FreeRTOS + mutex
```

### Diagrama de camadas

```
┌─────────────────────────────────┐
│           main.c                │  ← Orquestração apenas
├─────────────────────────────────┤
│        sensor_manager           │  ← Lógica: tarefa, mutex, dados
├─────────────────────────────────┤
│           hdc1080               │  ← Driver: I2C, registradores
├─────────────────────────────────┤
│     ESP-IDF (driver/i2c)        │  ← HAL do fabricante
└─────────────────────────────────┘
```

---

## Hardware

| Sinal     | ESP32 GPIO | HDC1080 |
|-----------|-----------|---------|
| SDA       | GPIO 21   | SDA     |
| SCL       | GPIO 22   | SCL     |
| Alimentação | 3.3V   | VCC     |
| GND       | GND       | GND     |

> Os pinos SDA e SCL podem ser alterados em `main/config.h`.

---

## Requisitos Atendidos

| Requisito                        | Implementação                                      |
|----------------------------------|----------------------------------------------------|
| ESP-IDF sem Arduino              | ✅ ESP-IDF nativo, `driver/i2c.h`                  |
| RTOS                             | ✅ FreeRTOS (incluso no ESP-IDF)                   |
| Tarefas paralelas                | ✅ `sensor_task` via `xTaskCreate()`               |
| Proteção de memória crítica      | ✅ Mutex `xSemaphoreCreateMutex()` em `s_sensor_data` |
| Interfaceamento com sensor       | ✅ HDC1080 via I2C (temperatura e umidade)         |
| Separação por camadas (SRP)      | ✅ `hdc1080`, `sensor_manager`, `main`             |
| Constantes em único local        | ✅ `main/config.h`                                 |
| Logs de depuração                | ✅ `ESP_LOGI/LOGE/LOGD` em todos os módulos        |
| Arquivos `.h` corretos           | ✅ Guards, `extern "C"`, apenas declarações        |
| Main simples                     | ✅ Apenas `sensor_manager_init()` + log            |
| Conectividade MQTT               | 🔲 Próxima etapa                                   |

---

## Boas Práticas Aplicadas

- **SRP**: cada módulo tem uma única responsabilidade.
- **Encapsulamento**: `s_sensor_data` nunca é acessado diretamente; use `sensor_manager_get_data()`.
- **Sem globais expostos**: estado interno dos módulos é `static`.
- **Período determinístico**: `vTaskDelayUntil()` garante leitura a cada 2s exatos.
- **Validação de hardware**: `hdc1080_init()` verifica o Manufacturer ID antes de prosseguir.
- **Tratamento de erros**: todos os retornos de função são verificados.

---

## Como Compilar

```bash
# Configure o target
idf.py set-target esp32

# Compile
idf.py build

# Grave e monitore
idf.py -p /dev/ttyUSB0 flash monitor
```

### Saída esperada no monitor serial

```
I (320)  MAIN: === Weather Station - Iniciando ===
I (325)  MAIN: IDF versão: v5.x.x
I (330)  HDC1080: Manufacturer ID lido: 0x5449 (esperado: 0x5449)
I (345)  HDC1080: HDC1080 inicializado com sucesso. I2C porta=0, SDA=21, SCL=22, freq=400000Hz
I (350)  SENSOR_MGR: Inicializando sensor_manager...
I (355)  SENSOR_MGR: sensor_manager inicializado com sucesso. Tarefa 'sensor_task' criada.
I (360)  SENSOR_MGR: Tarefa de leitura iniciada. Período: 2000ms, Prioridade: 5
I (2360) SENSOR_MGR: Leitura OK | Temperatura: 24.35°C | Umidade: 58.12%
I (4360) SENSOR_MGR: Leitura OK | Temperatura: 24.37°C | Umidade: 58.09%
```

---

## Próximas Etapas

- [ ] `wifi_manager`: conexão Wi-Fi com reconexão automática
- [ ] `mqtt_manager`: publicação dos dados via MQTT (broker HiveMQ ou Mosquitto)
- [ ] `http_server`: dashboard web local com atualização automática
- [ ] Leitura de pressão (ex: BMP280 via I2C no mesmo barramento)
