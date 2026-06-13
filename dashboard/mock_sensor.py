import paho.mqtt.client as mqtt
import json
import time
import random

MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPIC = "silve/temp-supervisory-sys/sensor-1/data"

def run_sensor():
    client = mqtt.Client()
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    print(f"Mock sensor started. Publishing to {MQTT_TOPIC}...")

    try:
        while True:
            # Generate a random temperature between 20 and 30
            temp = round(random.uniform(20.0, 30.0), 2)
            payload = json.dumps({"temperature": temp})
            
            client.publish(MQTT_TOPIC, payload)
            print(f"Published: {payload}")
            
            # Wait 5 seconds before next reading
            time.sleep(5)
    except KeyboardInterrupt:
        print("Sensor stopped.")
        client.disconnect()

if __name__ == "__main__":
    run_sensor()
