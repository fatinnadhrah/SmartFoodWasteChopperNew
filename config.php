<?php

$host = "localhost";
$user = "root";
$password = "";
$database = "smart_food_waste_chopper";

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