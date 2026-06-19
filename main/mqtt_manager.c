#include "mqtt_manager.h"
#include "config.h"
#include "mqtt_client.h"
#include "esp_log.h"

static const char *TAG = "MQTT_MANAGER";
static esp_mqtt_client_handle_t s_mqtt_client = NULL;
static bool s_connected = false;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Conectado ao Broker MQTT com sucesso.");
            s_connected = true;
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Desconectado do Broker MQTT.");
            s_connected = false;
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGD(TAG, "Mensagem publicada com sucesso, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "Ocorreu um erro na conexao MQTT");
            break;
        default:
            break;
    }
}

void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URL,
    };
    
    s_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (s_mqtt_client == NULL) {
        ESP_LOGE(TAG, "Falha ao inicializar o cliente MQTT.");
        return;
    }

    esp_mqtt_client_register_event(s_mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(s_mqtt_client);
}

bool mqtt_is_connected(void) {
    return s_connected;
}

esp_err_t mqtt_publish(const char *topic, const char *data) {
    if (s_mqtt_client == NULL || !s_connected) {
        return ESP_FAIL;
    }
    int msg_id = esp_mqtt_client_publish(s_mqtt_client, topic, data, 0, 1, 0); // QOS 1, retain 0
    if (msg_id >= 0) {
        ESP_LOGI(TAG, "Mensagem enviada com sucesso, msg_id=%d", msg_id);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Falha ao enviar mensagem MQTT.");
        return ESP_FAIL;
    }
}
