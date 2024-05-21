
-- Create the database
CREATE DATABASE sensor_network;

-- Use the created database
USE sensor_network;

-- Create the Sensor table
CREATE TABLE Sensor (
    id INT AUTO_INCREMENT PRIMARY KEY,
    type VARCHAR(45),
    status VARCHAR(45),
    ip_address VARCHAR(45),
    registration_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create the Component table
CREATE TABLE Component (
    id INT AUTO_INCREMENT PRIMARY KEY,
    error_count INT DEFAULT 0,
    last_substitution TIMESTAMP
);

-- Create the Telemetry table
CREATE TABLE Telemetry (
    id INT AUTO_INCREMENT PRIMARY KEY,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    vibration FLOAT NOT NULL,
    rotation FLOAT NOT NULL,
    pressure FLOAT NOT NULL,
    voltage FLOAT NOT NULL
);

-- Populate the Component table
INSERT INTO Component (error_count) VALUES (0);
INSERT INTO Component (error_count) VALUES (0);
INSERT INTO Component (error_count) VALUES (0);
INSERT INTO Component (error_count) VALUES (0);
INSERT INTO Component (error_count) VALUES (0);