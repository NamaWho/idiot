-- Create the database
CREATE DATABASE iot;

-- Use the created database
USE iot;

-- Create the sensor table
CREATE TABLE sensor (
    id INT AUTO_INCREMENT PRIMARY KEY,
    type VARCHAR(45),
    status VARCHAR(45),
    ip_address VARCHAR(45),
    registration_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create the telemetry table
CREATE TABLE telemetry (
    id INT AUTO_INCREMENT PRIMARY KEY,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    vibration FLOAT NOT NULL,
    rotation FLOAT NOT NULL,
    pressure FLOAT NOT NULL,
    voltage FLOAT NOT NULL
);