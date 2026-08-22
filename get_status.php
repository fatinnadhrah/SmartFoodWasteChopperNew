<?php

header("Content-Type: application/json");

require_once "config.php";

$sql = "
    SELECT
        machine_status,
        detection,
        lid_status,
        motor_status,
        remaining_time,
        updated_at
    FROM machine_status
    ORDER BY id DESC
    LIMIT 1
";

$result = $conn->query($sql);

if ($result && $result->num_rows > 0) {

    $data = $result->fetch_assoc();

    echo json_encode([
        "success" => true,
        "data" => $data
    ]);

} else {

    echo json_encode([
        "success" => false,
        "message" => "No status found"
    ]);

}

?>