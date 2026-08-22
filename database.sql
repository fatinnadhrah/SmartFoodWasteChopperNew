CREATE DATABASE IF NOT EXISTS smart_food_waste;

USE smart_food_waste;

CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    email VARCHAR(100) NOT NULL UNIQUE,
    password VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE machine_status (
    id INT AUTO_INCREMENT PRIMARY KEY,
    machine_status VARCHAR(30) NOT NULL DEFAULT 'READY',
    detection VARCHAR(10) NOT NULL DEFAULT 'NO',
    lid_status VARCHAR(30) NOT NULL DEFAULT 'UNLOCKED',
    motor_status VARCHAR(10) NOT NULL DEFAULT 'OFF',
    remaining_time INT NOT NULL DEFAULT 0,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        ON UPDATE CURRENT_TIMESTAMP
);

INSERT INTO machine_status
(
    machine_status,
    detection,
    lid_status,
    motor_status,
    remaining_time
)
VALUES
(
    'READY',
    'NO',
    'UNLOCKED',
    'OFF',
    0
);