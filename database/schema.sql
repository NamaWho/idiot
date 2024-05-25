-- Drop the database if it exists
DROP DATABASE IF EXISTS iot;

-- Create the database
CREATE DATABASE iot;

-- Use the created database
USE iot;

-- Drop the sensor table if it exists
DROP TABLE IF EXISTS sensor;

-- Create the sensor table
CREATE TABLE sensor (
    ip_address VARCHAR(45) PRIMARY KEY,
    port INT,
    type VARCHAR(45),
    status VARCHAR(45),
    registration_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Drop the telemetry table if it exists
DROP TABLE IF EXISTS telemetry;

-- Create the telemetry table
CREATE TABLE telemetry (
    id INT AUTO_INCREMENT PRIMARY KEY,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    vibration FLOAT NOT NULL,
    rotation FLOAT NOT NULL,
    pressure FLOAT NOT NULL,
    voltage FLOAT NOT NULL
);
