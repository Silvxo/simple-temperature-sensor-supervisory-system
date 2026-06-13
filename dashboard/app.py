import os
import json
import threading
from datetime import datetime, timedelta
from flask import Flask, render_template, jsonify
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
        # Extract sensor_id from topic: silve/temp-supervisory-sys/SENSOR_ID/data
        sensor_id = msg.topic.split('/')[2]

        if temperature is not None:
            with app.app_context():
                new_reading = SensorReading(sensor_id=sensor_id, temperature=temperature)
                db.session.add(new_reading)
                db.session.commit()
                print(f"Saved reading: {sensor_id} - {temperature}°C")
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
    # Fetch data from the last hour for the graph
    one_hour_ago = datetime.utcnow() - timedelta(hours=1)
    readings = SensorReading.query.filter(SensorReading.timestamp >= one_hour_ago).all()
    
    if not readings:
        return jsonify([])

    # Use Pandas for easy median calculation per minute
    df = pd.DataFrame([{
        'temperature': r.temperature,
        'timestamp': r.timestamp
    } for r in readings])
    
    df['timestamp'] = pd.to_datetime(df['timestamp'])
    df.set_index('timestamp', inplace=True)
    
    # Resample by minute and calculate median
    median_df = df['temperature'].resample('1min').median().dropna()
    
    result = [{"timestamp": ts.isoformat(), "temperature": temp} for ts, temp in median_df.items()]
    return jsonify(result)

if __name__ == '__main__':
    with app.app_context():
        db.create_all()
    
    # Start MQTT client in a background thread
    mqtt_thread = threading.Thread(target=start_mqtt, daemon=True)
    mqtt_thread.start()
    
    app.run(debug=True, port=5000)
