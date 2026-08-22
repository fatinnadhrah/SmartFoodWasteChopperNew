<?php

session_start();
require_once "config.php";

$message = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {

    $username = trim($_POST["username"]);
    $password = $_POST["password"];

    $stmt = $conn->prepare(
        "SELECT id, username, password
         FROM users
         WHERE username = ?"
    );

    $stmt->bind_param("s", $username);
    $stmt->execute();

    $result = $stmt->get_result();

    if ($result->num_rows == 1) {

        $user = $result->fetch_assoc();

        if (password_verify($password, $user["password"])) {

            session_regenerate_id(true);

            $_SESSION["user_id"] = $user["id"];
            $_SESSION["username"] = $user["username"];

            header("Location: index.php");
            exit();

        } else {

            $message = "Username atau password salah.";

        }

    } else {

        $message = "Username atau password salah.";

    }
}

?>

<!DOCTYPE html>
<html>
<head>

    <title>Login - Smart Food Waste Chopper</title>

    <link rel="stylesheet" href="style.css">

</head>

<body class="auth-page">

<div class="auth-box">

    <h1>SMART FOOD WASTE CHOPPER</h1>
    <h2>IoT Monitoring & Notification System</h2>

    <?php if (isset($_GET["signup"])): ?>

        <div class="success-message">
            Akaun berjaya dicipta. Sila login.
        </div>

    <?php endif; ?>

    <?php if ($message != ""): ?>

        <div class="error-message">
            <?php echo htmlspecialchars($message); ?>
        </div>

    <?php endif; ?>

    <form method="POST">

        <label>Username</label>

        <input
            type="text"
            name="username"
            required
        >

        <label>Password</label>

        <input
            type="password"
            name="password"
            required
        >

        <button type="submit" class="primary-btn">
            LOGIN
        </button>

    </form>

    <p>
        Don't have an account?
        <a href="signup.php">Create Account</a>
    </p>

</div>

</body>
</html>