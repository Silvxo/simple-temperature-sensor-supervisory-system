# Project Plan: ESP32 + HDC1080 Weather Station

Welcome! This document outlines the project structure and helps you specify your requirements. Please check/fill the sections below to let me know what we should work on next.


## 1. Project Context & Current Architecture
The project currently has two primary components:
1. **ESP32 Firmware ([main](file:///C:/Users/silve/codes/simple-temperature-sensor-supervisory-system/main))**:
   - Written in native ESP-IDF (C, FreeRTOS).
   - [hdc1080.c](file:///C:/Users/silve/codes/simple-temperature-sensor-supervisory-system/main/hdc1080.c) handles the sensor I2C driver logic.
   - [sensor_task.c](file:///C:/Users/silve/codes/simple-temperature-sensor-supervisory-system/main/sensor_task.c) reads temperature and humidity periodically.
   - Configuration is centralized in [config.h](file:///C:/Users/silve/codes/simple-temperature-sensor-supervisory-system/main/config.h).
2. **Supervisory Dashboard ([dashboard](file:///C:/Users/silve/codes/simple-temperature-sensor-supervisory-system/dashboard))**:
   - Python/Flask-based app (`app.py`) for collecting/displaying sensor data.
   - Database model using SQLAlchemy (`models.py`) writing to SQLite (`sensor_data.db`).
   - A mock sensor script (`mock_sensor.py`) to simulate hardware.

---

## 2. Next Steps / Feature Roadmap
Based on [README.md](file:///C:/Users/silve/codes/simple-temperature-sensor-supervisory-system/README.md), here are potential upcoming features. **Please mark the ones you want to implement with `[x]`:**

### ESP32 Firmware Enhancements
- [x] **Wi-Fi Manager**: Add Wi-Fi connection logic with automatic reconnection.
- [x] **MQTT Client**: Publish live temperature/humidity data from the ESP32 to an MQTT HiveMQ broker with the data specified in the app.py from the dashboard

The esp32 must connect in the wifi with ssid Sangsunga and password ibis2211

When the sensor reads the data, it stores the data with the current epoch time inside a file, that is inside esp32 persistent memory.
When it is time to send the data, the sendViaMQTT task must read from that file and send everything to the dashboard.
To make sure that both tasks are not using the file at the same time, implement a semaphore.

Make everything with the original Esp32 SDK

### Supervisory Dashboard Enhancements
- [x] **MQTT Broker Integration**: Update the Python Flask dashboard to subscribe to the ESP32's MQTT topic and store real data instead of mock data. The data will contain epoch time and the temperature data. Make sure the dashboard system can store the sensor data based on the time given by the sensor. Also, add a dropdown where the user can select the period he sees the temperature (15 minutes, 30 minutes, 1 hour, 6 hours, 12 hours, 24 hours)

### Other / Custom Requirements
- [ ] **Other**: *(Describe your custom requirement here)*

---

## 3. How to Provide Your Requirements
You can either:
1. Edit this [plan.md](file:///C:/Users/silve/.gemini/antigravity-cli/brain/1a764456-e4c6-43f1-a3f9-99207ac4fb87/plan.md) file directly to mark your selections and write comments, and then let me know.
2. Reply in the chat detailing which features you want to focus on first, or any other changes you'd like.


### MQTT Settings 
Host: 	broker.hivemq.com
TCP Port: 	1883
Websocket Port: 	8000
TLS TCP Port: 	8883
TLS Websocket Port: 	8884
