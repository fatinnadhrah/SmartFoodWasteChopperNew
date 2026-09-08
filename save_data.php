<?php

require_once "config.php";

// Check whether the request comes from ESP32
if ($_SERVER["REQUEST_METHOD"] !== "POST") {
    echo "Invalid request method";
    exit;
}

// Get data sent by ESP32
$machine_status = $_POST["machine_status"] ?? "";
$detection = $_POST["detection"] ?? "";
$lid_status = $_POST["lid_status"] ?? "";
$motor_status = $_POST["motor_status"] ?? "";
$remaining_time = isset($_POST["remaining_time"])
    ? intval($_POST["remaining_time"])
    : 0;

// Check required data
if (
    $machine_status === "" ||
    $detection === "" ||
    $lid_status === "" ||
    $motor_status === ""
) {
    echo "Missing data";
    exit;
}

// Prepare SQL statement
$sql = "INSERT INTO machine_status
        (machine_status, detection, lid_status, motor_status, remaining_time)
        VALUES (?, ?, ?, ?, ?)";

$stmt = $conn->prepare($sql);

if (!$stmt) {
    echo "SQL prepare failed: " . $conn->error;
    exit;
}

// Bind data
$stmt->bind_param(
    "ssssi",
    $machine_status,
    $detection,
    $lid_status,
    $motor_status,
    $remaining_time
);

// Execute
if ($stmt->execute()) {

    echo "Data saved successfully";

} else {

    echo "Failed to save data: " . $stmt->error;
}

$stmt->close();
$conn->close();

?>