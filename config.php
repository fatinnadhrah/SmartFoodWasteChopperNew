<?php

$host = "localhost";
$user = "root";
$password = "";
$database = "smart_food_waste_chopper";

// IP address of the ESP32 on your local Wi-Fi network.
// Find this from the Serial Monitor after the ESP32 boots -
// it prints a line like "ESP32 IP Address: 192.168.1.50".
// CHANGE THIS to match your own ESP32's IP.
define("ESP32_IP", "192.168.1.50");

$conn = new mysqli(
    $host,
    $user,
    $password,
    $database
);

if ($conn->connect_error) {
    die("Database connection failed: " . $conn->connect_error);
}

$conn->set_charset("utf8mb4");

?>