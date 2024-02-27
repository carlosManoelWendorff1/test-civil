# services.py

from decimal import Decimal
from flask import Flask, request, jsonify
from sqlalchemy import Column, Integer, String, DateTime, Float, Boolean, ForeignKey,create_engine
from sqlalchemy.orm import relationship
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from datetime import datetime

from models.meter import Meter, Reading, Sensor

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

class ReadingService:
    @staticmethod
    def create_reading(value, time, sensor_id):
        session = DBSession()
        reading = Reading(value=value, time=time, sensor_id=sensor_id)
        session.add(reading)
        session.commit()
        session.close()
        return reading.id

    @staticmethod
    def get_readings():
        session = DBSession()
        readings = session.query(Reading).all()
        session.close()
        return [{"id": reading.id, "value": reading.value, "time": reading.time, "sensor_id": reading.sensor_id} for reading in readings]

    @staticmethod
    def get_sensor_readings(sensor_id):
        session = DBSession()
        readings = session.query(Reading).filter_by(sensor_id=sensor_id).all()
        session.close()
        return [{"id": reading.id, "value": reading.value, "time": reading.time} for reading in readings]

    # Implement update and delete methods for Reading, if needed
class SensorService:
    @staticmethod
    def create_sensor(isDefault,meter_id):
        session = DBSession()
        sensor = Sensor(isDefault=isDefault, meter_id=meter_id)
        session.add(sensor)
        session.commit()
        session.close()
        return sensor.id

    @staticmethod
    def get_sensors():
        session = DBSession()
        sensors = session.query(Sensor).all()
        session.close()
        return [{"id": sensor.id, "isDefault": sensor.isDefault, "time": sensor.time, "meter_id": sensor.meter_id} for sensor in sensors]

    @staticmethod
    def get_meter_sensors(meter_id):
        session = DBSession()
        sensors = session.query(Sensor).filter_by(meter_id=meter_id).all()
        session.close()
        return [{"id": sensor.id, "isDefault": sensor.isDefault, "time": sensor.time} for sensor in sensors]

class MeterService:
    @staticmethod
    def create_meter(battery):
        session = DBSession()
        meter = Meter(battery=battery)
        session.add(meter)
        session.commit()
        return meter.id

    @staticmethod
    def get_meters():
        session = DBSession()
        meters = session.query(Meter).all()
        session.close()
        return [{"id": meter.id, "battery": meter.battery} for meter in meters]

    @staticmethod
    def get_meter(meter_id):
        session = DBSession()
        meter = session.query(Meter).get(meter_id)
        session.close()
        if meter:
            return {"id": meter.id, "battery": meter.battery}
        else:
            return None

    @staticmethod
    def update_meter(meter_id, new_battery):
        session = DBSession()
        meter = session.query(Meter).get(meter_id)
        if meter:
            meter.battery = new_battery
            session.commit()
            session.close()
            return {"message": "Meter updated successfully", "meter_id": meter.id}
        else:
            session.close()
            return None

    @staticmethod
    def delete_meter(meter_id):
        session = DBSession()
        meter = session.query(Meter).get(meter_id)
        if meter:
            session.delete(meter)
            session.commit()
            session.close()
            return {"message": "Meter deleted successfully", "meter_id": meter.id}
        else:
            session.close()
            return None

