from datetime import datetime
from sqlalchemy import Column, Integer, String, DateTime, Float, ForeignKey
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()

class Reading(Base):
    __tablename__ = 'reading'

    id = Column(Integer, primary_key=True)
    value = Column(Float, nullable=False)
    time = Column(DateTime, default=datetime.utcnow, nullable=False)
    type = Column(String, nullable=False)
    sensor_id = Column(Integer, ForeignKey('sensor.id'))
