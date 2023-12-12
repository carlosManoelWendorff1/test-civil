from sqlalchemy import Column, Integer, String, DateTime, Float, Boolean, ForeignKey,create_engine
from sqlalchemy.orm import relationship
from sqlalchemy.ext.declarative import declarative_base

from civil import Base


class Sensor(Base):
    __tablename__ = 'sensor'

    id = Column(Integer, primary_key=True)
    isDefault = Column(Boolean, default=False)
    meter_id = Column(Integer, ForeignKey('meter.id'))

    readings = relationship("Reading")