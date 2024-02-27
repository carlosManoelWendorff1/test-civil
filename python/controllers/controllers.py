# controllers.py
from flask import Flask, request, jsonify
from services.services import MeterService, ReadingService, SensorService
from flask import Blueprint

controllers_blueprint = Blueprint('controllers', __name__)

app = Flask(__name__)

# Initialize services
meter_service = MeterService()
reading_service = ReadingService()
sensor_service = SensorService()



@controllers_blueprint.route('/meters', methods=['POST'])
def create_meter():
    data = request.get_json()
    if "battery" in data:
        battery = data["battery"]
        meter_id = meter_service.create_meter(battery)
        return jsonify({"message": "Meter created successfully", "meter_id": meter_id}), 201
    else:
        return jsonify({"error": "Missing 'battery' value"}), 400

@controllers_blueprint.route('/meters', methods=['GET'])
def get_meters():
    meters = meter_service.get_meters()
    return jsonify(meters)

@controllers_blueprint.route('/meters/<int:meter_id>', methods=['GET'])
def get_meter(meter_id):
    meter = meter_service.get_meter(meter_id)
    if meter:
        return jsonify(meter)
    else:
        return jsonify({"error": "Meter not found"}), 404

@controllers_blueprint.route('/meters/<int:meter_id>', methods=['PUT'])
def update_meter(meter_id):
    data = request.get_json()
    new_battery = data.get("battery")
    result = meter_service.update_meter(meter_id, new_battery)
    if result:
        return jsonify(result)
    else:
        return jsonify({"error": "Meter not found"}), 404

@controllers_blueprint.route('/meters/<int:meter_id>', methods=['DELETE'])
def delete_meter(meter_id):
    result = meter_service.delete_meter(meter_id)
    if result:
        return jsonify(result)
    else:
        return jsonify({"error": "Meter not found"}), 404

@controllers_blueprint.route('/readings', methods=['POST'])
def create_reading():
    data = request.get_json()
    if "value" in data and "time" in data and "sensor_id" in data:
        value = data["value"]
        time = data["time"]
        sensor_id = data["sensor_id"]
        reading_id = reading_service.create_reading(value, time, sensor_id)
        return jsonify({"message": "Reading created successfully", "reading_id": reading_id}), 201
    else:
        return jsonify({"error": "Missing values"}), 400

@controllers_blueprint.route('/readings', methods=['GET'])
def get_readings():
    readings = reading_service.get_readings()
    return jsonify(readings)

@controllers_blueprint.route('/sensors/<int:sensor_id>/readings', methods=['GET'])
def get_sensor_readings(sensor_id):
    readings = reading_service.get_sensor_readings(sensor_id)
    return jsonify(readings)

@controllers_blueprint.route('/sensors', methods=['POST'])
def create_sensor():
    data = request.get_json()
    if "isDefault" in data and "meter_id" in data:
        isDefault = data["isDefault"]
        meter_id = data
        sensor_id = sensor_service.create_sensor(isDefault,meter_id)
        return jsonify(sensor_id)
    else:
        return jsonify("bad Request")