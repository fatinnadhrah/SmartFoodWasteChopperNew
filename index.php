<?php

session_start();

if (!isset($_SESSION["user_id"])) {

    header("Location: login.php");
    exit();

}

$username = $_SESSION["username"];

?>

<!DOCTYPE html>
<html>

<head>

    <meta charset="UTF-8">

    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <title>
        Smart Food Waste Chopper With IoT Monitoring & Notification System
    </title>

    <link rel="stylesheet" href="style.css">

</head>

<body class="body">

    <div class="dashboard">

        <header class="topbar">

            <div>

                <h1>
                    SMART FOOD WASTE CHOPPER
                </h1>

                <p>
                    IoT MONITORING & NOTIFICATION SYSTEM
                </p>

            </div>

            <div class="user-area">

                <span>
                    User: <?php echo htmlspecialchars($username); ?>
                </span>

                <a href="logout.php" class="logout-btn">
                    LOGOUT
                </a>

            </div>

        </header>


        <main>

            <section class="status-grid">

                <div class="status-card">

                    <span class="label">
                        MACHINE STATUS
                    </span>

                    <strong id="machineStatus">
                        READY
                    </strong>

                </div>


                <div class="status-card">

                    <span class="label">
                        FOOD DETECTION
                    </span>

                    <strong id="detectionStatus">
                        NO
                    </strong>

                </div>


                <div class="status-card">

                    <span class="label">
                        LID STATUS
                    </span>

                    <strong id="lidStatus">
                        UNLOCKED
                    </strong>

                </div>


                <div class="status-card">

                    <span class="label">
                        MOTOR STATUS
                    </span>

                    <strong id="motorStatus">
                        OFF
                    </strong>

                </div>

            </section>


            <section class="process-panel">

                <h2>
                    PROCESS MONITORING
                </h2>

                <div class="timer">

                    <span id="timer">
                        00
                    </span>

                    <small>
                        SECONDS REMAINING
                    </small>

                </div>


                <div class="progress-container">

                    <div class="progress-bar" id="progressBar">
                    </div>

                </div>


                <div class="buttons">

                    <button id="doneBtn" onclick="startProcess()">

                        DONE

                    </button>


                    <button id="choppingBtn" onclick="continueChopping()">

                        CHOPPING

                    </button>

                </div>

            </section>


            <section class="log-panel">

                <h2>
                    SYSTEM EVENT LOG
                </h2>

                <div id="eventLog">

                    <p>
                        System initialized.
                    </p>

                </div>

            </section>

        </main>

    </div>


    <div id="notification" class="notification">

        PROCESS COMPLETED!

    </div>


    <script src="script.js"></script>

</body>

</html>