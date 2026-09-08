<?php

session_start();
require_once "config.php";

header("Content-Type: application/json");

if (!isset($_SESSION["user_id"])) {
    http_response_code(401);
    echo json_encode([
        "success" => false,
        "message" => "Please login first."
    ]);
    exit;
}

// ------------------------------------------------
// Call an endpoint on the ESP32 over Wi-Fi (HTTP)
// ------------------------------------------------
function callEsp32($path) {

    $url = "http://" . ESP32_IP . $path;

    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 2);
    curl_setopt($ch, CURLOPT_TIMEOUT, 3);

    $response = curl_exec($ch);
    $ok = ($response !== false) && (curl_errno($ch) === 0);
    $error = curl_error($ch);

    curl_close($ch);

    return [$ok, $response, $error];
}

// ------------------------------------------------
// Save / read the last known sensor state in MySQL
// (used as a fallback if the ESP32 is unreachable)
// ------------------------------------------------
function saveSensorState($conn, $enabled) {
    $val = $enabled ? 1 : 0;

    $stmt = $conn->prepare(
        "INSERT INTO sensor_status (sensor_enabled) VALUES (?)"
    );
    $stmt->bind_param("i", $val);
    $stmt->execute();
    $stmt->close();
}

function getLastSensorState($conn) {
    $result = $conn->query(
        "SELECT sensor_enabled FROM sensor_status ORDER BY id DESC LIMIT 1"
    );

    if ($result && $row = $result->fetch_assoc()) {
        return (bool) $row["sensor_enabled"];
    }

    return false;
}

// ------------------------------------------------
// GET ?get=sensor -> report current sensor state
// ------------------------------------------------
if ($_SERVER["REQUEST_METHOD"] === "GET" && ($_GET["get"] ?? "") === "sensor") {

    [$ok, $response] = callEsp32("/sensor/status");

    if ($ok) {
        $enabled = (trim($response) === "ON");
        saveSensorState($conn, $enabled);
    } else {
        // ESP32 not reachable right now - fall back to the
        // last state we saved in the database.
        $enabled = getLastSensorState($conn);
    }

    echo json_encode([
        "success" => true,
        "sensor_enabled" => $enabled
    ]);

    $conn->close();
    exit;
}

// ------------------------------------------------
// POST command=SENSOR_ON / SENSOR_OFF
// ------------------------------------------------
if ($_SERVER["REQUEST_METHOD"] === "POST") {

    $command = $_POST["command"] ?? "";

    if (!in_array($command, ["SENSOR_ON", "SENSOR_OFF"], true)) {
        echo json_encode([
            "success" => false,
            "message" => "Invalid command"
        ]);
        exit;
    }

    $endpoint = ($command === "SENSOR_ON") ? "/sensor/on" : "/sensor/off";

    [$ok, , $error] = callEsp32($endpoint);

    if (!$ok) {
        echo json_encode([
            "success" => false,
            "message" => "Cannot reach ESP32 at " . ESP32_IP . " (" . $error . "). " .
                          "Make sure the ESP32 and this server are on the same Wi-Fi network."
        ]);
        $conn->close();
        exit;
    }

    $enabled = ($command === "SENSOR_ON");
    saveSensorState($conn, $enabled);

    echo json_encode([
        "success" => true,
        "sensor_enabled" => $enabled
    ]);

    $conn->close();
    exit;
}

echo json_encode([
    "success" => false,
    "message" => "Invalid request"
]);

$conn->close();

?>