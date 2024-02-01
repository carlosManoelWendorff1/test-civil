# app.py
from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_migrate import Migrate
from flask_cors import CORS
from controllers import controllers_blueprint

from decimal import Decimal
from flask import Flask, request, jsonify
from sqlalchemy import Column, Integer, String, DateTime, Float, Boolean, ForeignKey,create_engine
from sqlalchemy.orm import relationship
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from datetime import datetime
from models import meter


# Initialize the Flask app
app = Flask(__name__)

# Configure the app
app.config['SQLALCHEMY_DATABASE_URI'] = 'postgresql://postgres_user:12345678@localhost:5432/postgres_user'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

# If DATABASE_URL is not set, use default connection (MySQL)
db_username = "postgres_user"
db_password = "12345678"
db_host = "localhost"
db_port = "5432"
db_name = "postgres_user"

connection_string = f'postgresql://{db_username}:{db_password}@{db_host}:{db_port}/{db_name}'
engine = create_engine(connection_string)
Base = declarative_base()
Session = sessionmaker(bind=engine)

# Create the database tables
Base.metadata.create_all(engine)

# Initialize the database session
session = Session()

# Fetch a Reading instance from the database
meters = meter.query.first()
print("teste")
print(meters)

# Initialize the database and migrate
db = SQLAlchemy(app)
migrate = Migrate(app, db)

# Enable CORS
CORS(app)

# Register the controllers
app.register_blueprint(controllers_blueprint, url_prefix='/api')

# Run the app
if __name__ == '__main__':
    app.run(debug=True)