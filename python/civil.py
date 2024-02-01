
from decimal import Decimal
from flask import Flask, request, jsonify
from sqlalchemy import Column, Integer, String, DateTime, Float, Boolean, ForeignKey,create_engine
from sqlalchemy.orm import relationship
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from datetime import datetime

from models.meter import Meter
from models.reading import Reading
from models.sensor import Sensor

app = Flask(__name__)

# If DATABASE_URL is not set, use default connection (MySQL)
db_username = "postgres_user"
db_password = "12345678"
db_host = "localhost"
db_port = "5432"
db_name = "postgres_user"

connection_string = f'postgresql://{db_username}:{db_password}@{db_host}:{db_port}/{db_name}'
engine = create_engine(connection_string)
Base = declarative_base()
DBSession = sessionmaker(bind=engine)
session = DBSession()


# Create the database tables
Base.metadata.create_all(engine)
# Assuming you've already set up your Flask app, SQLAlchemy models, and database connection

# Create a new meter
@app.route('/meters', methods=['POST'])
def create_meter():
    data = request.get_json()
    if "battery" in data:
        battery = data["battery"]
        meter = Meter(battery=battery)
        session.add(meter)
        session.commit()
        return jsonify({"message": "Meter created successfully", "meter_id": meter.id}), 201
    else:
        return jsonify({"error": "Missing 'battery' value"}), 400

# Get all meters
@app.route('/meters', methods=['GET'])
def get_meters():
    meters = session.query(Meter).all()
    meter_dicts = [{"id": meter.id, "battery": meter.battery} for meter in meters]
    return jsonify(meter_dicts)

# Get a specific meter by ID
@app.route('/meters/<int:meter_id>', methods=['GET'])
def get_meter(meter_id):
    meter = session.query(Meter).get(meter_id)
    if meter:
        return jsonify({"id": meter.id, "battery": meter.battery})
    else:
        return jsonify({"error": "Meter not found"}), 404

# Update a meter's battery value by ID
@app.route('/meters/<int:meter_id>', methods=['PUT'])
def update_meter(meter_id):
    data = request.get_json()
    new_battery = data.get("battery")
    meter = session.query(Meter).get(meter_id)
    if meter:
        if new_battery is not None:
            meter.battery = new_battery
            session.commit()
            return jsonify({"message": "Meter updated successfully", "meter_id": meter.id}), 200
        else:
            return jsonify({"error": "Missing 'battery' value"}), 400
    else:
        return jsonify({"error": "Meter not found"}), 404

# Delete a meter by ID
@app.route('/meters/<int:meter_id>', methods=['DELETE'])
def delete_meter(meter_id):
    meter = session.query(Meter).get(meter_id)
    if meter:
        session.delete(meter)
        session.commit()
        return jsonify({"message": "Meter deleted successfully", "meter_id": meter.id}), 200
    else:
        return jsonify({"error": "Meter not found"}), 404

# Create a new reading for a sensor
@app.route('/readings', methods=['POST'])
def create_reading():
    data = request.get_json()
    if "value" in data and "time" in data and "sensor_id" in data:
        value = data["value"]
        time = data["time"]
        sensor_id = data["sensor_id"]
        reading = Reading(value=value, time=time, sensor_id=sensor_id)
        session.add(reading)
        session.commit()
        return jsonify({"message": "Reading created successfully", "reading_id": reading.id}), 201
    else:
        return jsonify({"error": "Missing values"}), 400

# Get all readings
@app.route('/readings', methods=['GET'])
def get_readings():
    readings = session.query(Reading).all()
    reading_dicts = [{"id": reading.id, "value": reading.value, "time": reading.time, "sensor_id": reading.sensor_id} for reading in readings]
    return jsonify(reading_dicts)

# Get readings for a specific sensor
@app.route('/sensors/<int:sensor_id>/readings', methods=['GET'])
def get_sensor_readings(sensor_id):
    sensor = session.query(Sensor).get(sensor_id)
    if sensor:
        readings = session.query(Reading).filter_by(sensor_id=sensor_id).all()
        reading_dicts = [{"id": reading.id, "value": reading.value, "time": reading.time} for reading in readings]
        return jsonify(reading_dicts)
    else:
        return jsonify({"error": "Sensor not found"}), 404

# Other CRUD operations for Reading (Update and Delete) can be implemented similarly

# Create a new sensor for a meter
@app.route('/sensors', methods=['POST'])
def create_sensor():
    data = request.get_json()
    if "isDefault" in data and "time" in data and "meter_id" in data:
        isDefault = data["isDefault"]
        time = data["time"]
        meter_id = data["meter_id"]
        sensor = Sensor(isDefault=isDefault, time=time, meter_id=meter_id)
        session.add(sensor)
        session.commit()
        return jsonify({"message": "Sensor created successfully", "sensor_id": sensor.id}), 201
    else:
        return jsonify({"error": "Missing values"}), 400

# Get all sensors
@app.route('/sensors', methods=['GET'])
def get_sensors():
    sensors = session.query(Sensor).all()
    sensor_dicts = [{"id": sensor.id, "isDefault": sensor.isDefault, "time": sensor.time, "meter_id": sensor.meter_id} for sensor in sensors]
    return jsonify(sensor_dicts)

# Get sensors for a specific meter
@app.route('/meters/<int:meter_id>/sensors', methods=['GET'])
def get_meter_sensors(meter_id):
    meter = session.query(Meter).get(meter_id)
    if meter:
        sensors = session.query(Sensor).filter_by(meter_id=meter_id).all()
        sensor_dicts = [{"id": sensor.id, "isDefault": sensor.isDefault, "time": sensor.time} for sensor in sensors]
        return jsonify(sensor_dicts)
    else:
        return jsonify({"error": "Meter not found"}), 404

# Other CRUD operations for Sensor (Update and Delete) can be implemented similarly


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
