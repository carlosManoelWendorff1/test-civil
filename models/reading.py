from datetime import datetime
from sqlalchemy import Column, Integer, String, DateTime, Float, Boolean, ForeignKey,create_engine
from sqlalchemy.orm import relationship
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()
class Reading(Base):
    __tablename__ = 'reading'

    id = Column(Integer, primary_key=True)
    value = Column(Float(2),nullable=False)
    time = Column(DateTime, default=datetime.utcnow)
    type = Column(String,nullable=False)
    sensor_id = Column(Integer, ForeignKey('sensor.id'))