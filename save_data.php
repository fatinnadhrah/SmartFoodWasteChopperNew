<?php

header("Content-Type: application/json");

require_once "config.php";

$machine =
    $_POST["machine_status"] ?? "READY";

$detection =
    $_POST["detection"] ?? "NO";

$lid =
    $_POST["lid_status"] ?? "UNLOCKED";

$motor =
    $_POST["motor_status"] ?? "OFF";

$remaining =
    intval($_POST["remaining_time"] ?? 0);


$stmt = $conn->prepare("
    INSERT INTO machine_status
    (
        machine_status,
        detection,
        lid_status,
        motor_status,
        remaining_time
    )
    VALUES (?, ?, ?, ?, ?)
");


$stmt->bind_param(
    "ssssi",
    $machine,
    $detection,
    $lid,
    $motor,
    $remaining
);


if ($stmt->execute()) {

    echo json_encode([
        "success" => true,
        "message" => "Data saved"
    ]);

} else {

    echo json_encode([
        "success" => false,
        "message" => "Failed to save data"
    ]);

}

?>