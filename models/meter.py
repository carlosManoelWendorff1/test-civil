from sqlalchemy import Column, Integer, String, DateTime, Float, Boolean, ForeignKey,create_engine
from sqlalchemy.orm import relationship
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()

class Meter(Base):
    __tablename__ = 'meter'

    id = Column(Integer, primary_key=True)
    battery = Column(Integer)

    sensors = relationship("Sensor", back_populates="meter")