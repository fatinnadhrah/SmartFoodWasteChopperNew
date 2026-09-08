let processTimer = null;

let remaining = 0;

const LOCK_COUNTDOWN = 20;

const CHOP_TIME = 30;
// ===============================
// SENSOR CONTROL
// ===============================

let sensorEnabled = false;


// ===============================
// SENSOR ON / OFF
// ===============================

function toggleSensor() {

    const button = document.getElementById("sensorBtn");
    const message = document.getElementById("sensorMessage");

    const command = sensorEnabled
        ? "SENSOR_OFF"
        : "SENSOR_ON";


    fetch("command.php", {

        method: "POST",

        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },

        body: "command=" + command

    })

    .then(response => response.json())

    .then(data => {

        if (data.success) {

            sensorEnabled = data.sensor_enabled;

            if (sensorEnabled) {

                button.textContent = "SENSOR ON";

                button.classList.remove("sensor-off");

                button.classList.add("sensor-on");

                message.textContent =
                    "Ultrasonic Sensor is ON";

            } else {

                button.textContent = "SENSOR OFF";

                button.classList.remove("sensor-on");

                button.classList.add("sensor-off");

                message.textContent =
                    "Ultrasonic Sensor is OFF";
            }

        }

    })

    .catch(error => {

        console.error("Sensor error:", error);

    });
}


// ===============================
// CHECK SENSOR STATUS
// ===============================

function loadSensorStatus() {

    fetch("command.php?get=sensor")

    .then(response => response.json())

    .then(data => {

        if (data.success) {

            sensorEnabled = data.sensor_enabled;

            const button =
                document.getElementById("sensorBtn");

            const message =
                document.getElementById("sensorMessage");


            if (sensorEnabled) {

                button.textContent = "SENSOR ON";

                button.classList.remove("sensor-off");

                button.classList.add("sensor-on");

                message.textContent =
                    "Ultrasonic Sensor is ON";

            } else {

                button.textContent = "SENSOR OFF";

                button.classList.remove("sensor-on");

                button.classList.add("sensor-off");

                message.textContent =
                    "Ultrasonic Sensor is OFF";
            }

        }

    })

    .catch(error => {

        console.error("Cannot get sensor status:", error);

    });
}


// Check sensor status when page opens

document.addEventListener(
    "DOMContentLoaded",
    loadSensorStatus
);

function updateStatus(
    machine,
    detection,
    lid,
    motor
) {

    document.getElementById(
        "machineStatus"
    ).textContent = machine;

    document.getElementById(
        "detectionStatus"
    ).textContent = detection;

    document.getElementById(
        "lidStatus"
    ).textContent = lid;

    document.getElementById(
        "motorStatus"
    ).textContent = motor;

}


function addLog(message) {

    const log =
        document.getElementById("eventLog");

    const time =
        new Date().toLocaleTimeString();

    const p =
        document.createElement("p");

    p.textContent =
        time + " - " + message;

    log.prepend(p);
}


function startProcess() {

    if (processTimer !== null) {
        return;
    }

    addLog(
        "DONE button pressed."
    );

    updateStatus(
        "PROCESS",
        "YES",
        "LOCKED",
        "ON"
    );

    startChoppingCountdown();
}


function startChoppingCountdown() {

    remaining = CHOP_TIME;

    updateTimer();

    processTimer =
        setInterval(() => {

            remaining--;

            updateTimer();

            if (remaining <= 0) {

                completeProcess();

            }

        }, 1000);
}


function continueChopping() {

    if (processTimer === null) {

        addLog(
            "Chopping button pressed."
        );

        updateStatus(
            "PROCESS",
            "YES",
            "LOCKED",
            "ON"
        );

        startChoppingCountdown();

    } else {

        addLog(
            "Chopping process continues."
        );

    }
}


function updateTimer() {

    document.getElementById(
        "timer"
    ).textContent =
        String(remaining).padStart(2, "0");

    const progress =
        ((CHOP_TIME - remaining)
        / CHOP_TIME) * 100;

    document.getElementById(
        "progressBar"
    ).style.width =
        progress + "%";
}


function completeProcess() {

    clearInterval(processTimer);

    processTimer = null;

    remaining = 0;

    updateTimer();

    updateStatus(
        "COMPLETED",
        "YES",
        "UNLOCKED",
        "OFF"
    );

    addLog(
        "PROCESS COMPLETED."
    );

    showNotification();
}


function showNotification() {

    const notification =
        document.getElementById(
            "notification"
        );

    notification.style.display =
        "block";

    setTimeout(() => {

        notification.style.display =
            "none";

    }, 6000);
}