<?php

header("Content-Type: application/json");

require_once "config.php";

$command =
    $_POST["command"] ?? "";


if ($command == "DONE") {

    $machine = "PROCESS";
    $detection = "YES";
    $lid = "LOCKED";
    $motor = "ON";
    $remaining = 30;

} elseif ($command == "STOP") {

    $machine = "COMPLETED";
    $detection = "YES";
    $lid = "UNLOCKED";
    $motor = "OFF";
    $remaining = 0;

} else {

    echo json_encode([
        "success" => false,
        "message" => "Invalid command"
    ]);

    exit();

}


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
        "command" => $command
    ]);

} else {

    echo json_encode([
        "success" => false
    ]);

}

?>