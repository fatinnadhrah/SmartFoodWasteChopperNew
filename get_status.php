<?php

require_once "config.php";

// Get the latest machine status
$sql = "SELECT 
            id,
            machine_status,
            detection,
            lid_status,
            motor_status,
            remaining_time,
            updated_at
        FROM machine_status
        ORDER BY id DESC
        LIMIT 1";

$result = $conn->query($sql);

if (!$result) {
    http_response_code(500);

    echo json_encode([
        "success" => false,
        "message" => "Database query failed"
    ]);

    exit;
}

// Check whether data exists
if ($result->num_rows > 0) {

    $row = $result->fetch_assoc();

    header("Content-Type: application/json");

    echo json_encode([
        "success" => true,
        "data" => [
            "id" => $row["id"],
            "machine_status" => $row["machine_status"],
            "detection" => $row["detection"],
            "lid_status" => $row["lid_status"],
            "motor_status" => $row["motor_status"],
            "remaining_time" => (int)$row["remaining_time"],
            "updated_at" => $row["updated_at"]
        ]
    ]);

} else {

    header("Content-Type: application/json");

    echo json_encode([
        "success" => false,
        "message" => "No machine status found"
    ]);
}

$conn->close();

?>