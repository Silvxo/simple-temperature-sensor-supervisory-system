import os
import json
import threading
from datetime import datetime, timedelta
from flask import Flask, render_template, jsonify, request
from models import db, SensorReading
import paho.mqtt.client as mqtt
import pandas as pd

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_DATA_DB_URI'] = 'sqlite:///sensor_data.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

# Ensure the database URI is correct for the directory
basedir = os.path.abspath(os.path.dirname(__file__))
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///' + os.path.join(basedir, 'sensor_data.db')

db.init_app(app)

# MQTT Configuration
MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPIC = "silve/temp-supervisory-sys/+/data" # Using wildcard for multiple sensors

def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT broker with result code {rc}")
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        temperature = payload.get("temperature")
        timestamp_epoch = payload.get("timestamp")
        # Extract sensor_id from topic: silve/temp-supervisory-sys/SENSOR_ID/data
        sensor_id = msg.topic.split('/')[2]

        if temperature is not None:
            with app.app_context():
                # Converte epoch time enviado pelo ESP32 para datetime UTC
                if timestamp_epoch is not None:
                    dt = datetime.utcfromtimestamp(timestamp_epoch)
                else:
                    dt = datetime.utcnow()
                
                new_reading = SensorReading(sensor_id=sensor_id, temperature=temperature, timestamp=dt)
                db.session.add(new_reading)
                db.session.commit()
                print(f"Saved reading: {sensor_id} - {temperature}°C at {dt} UTC")
    except Exception as e:
        print(f"Error processing MQTT message: {e}")

def start_mqtt():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.loop_forever()

# Routes
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/latest')
def get_latest():
    latest = SensorReading.query.order_by(SensorReading.timestamp.desc()).first()
    return jsonify(latest.to_dict() if latest else {"error": "No data yet"})

@app.route('/api/history')
def get_history():
    readings = SensorReading.query.order_by(SensorReading.timestamp.desc()).limit(100).all()
    return jsonify([r.to_dict() for r in readings])

@app.route('/api/median_per_minute')
def get_median():
    # Obtém o período solicitado do dropdown (default: 1 hora)
    period = request.args.get('period', '1h')
    
    # Mapeia o período para o time window e a frequência de reamostragem (resample)
    period_map = {
        '15m': (timedelta(minutes=15), '30s'),
        '30m': (timedelta(minutes=30), '1min'),
        '1h': (timedelta(hours=1), '1min'),
        '6h': (timedelta(hours=6), '5min'),
        '12h': (timedelta(hours=12), '10min'),
        '24h': (timedelta(hours=24), '15min'),
    }
    
    time_window, resample_freq = period_map.get(period, (timedelta(hours=1), '1min'))
    
    # Filtra usando UTC (combinando com timestamp gravado)
    start_time = datetime.utcnow() - time_window
    readings = SensorReading.query.filter(SensorReading.timestamp >= start_time).all()
    
    if not readings:
        return jsonify([])

    # Utiliza Pandas para calcular a mediana
    df = pd.DataFrame([{
        'temperature': r.temperature,
        'timestamp': r.timestamp
    } for r in readings])
    
    df['timestamp'] = pd.to_datetime(df['timestamp'])
    df.set_index('timestamp', inplace=True)
    
    # Reamostra pela frequência mapeada e calcula a mediana
    median_df = df['temperature'].resample(resample_freq).median().dropna()
    
    result = [{"timestamp": ts.isoformat() + 'Z', "temperature": temp} for ts, temp in median_df.items()]
    return jsonify(result)


if __name__ == '__main__':
    with app.app_context():
        db.create_all()
    
    # Inicia MQTT em background
    mqtt_thread = threading.Thread(target=start_mqtt, daemon=True)
    mqtt_thread.start()
    
    app.run(debug=True, port=5000)

