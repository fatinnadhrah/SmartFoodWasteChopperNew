<?php

session_start();
require_once "config.php";

$message = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {

    $username = trim($_POST["username"]);
    $email = trim($_POST["email"]);
    $password = $_POST["password"];

    if ($username == "" || $email == "" || $password == "") {

        $message = "Sila lengkapkan semua maklumat.";

    } elseif (strlen($password) < 6) {

        $message = "Password mestilah sekurang-kurangnya 6 aksara.";

    } else {

        $check = $conn->prepare(
            "SELECT id FROM users WHERE username = ? OR email = ?"
        );

        $check->bind_param("ss", $username, $email);
        $check->execute();
        $result = $check->get_result();

        if ($result->num_rows > 0) {

            $message = "Username atau email sudah digunakan.";

        } else {

            $hashedPassword = password_hash(
                $password,
                PASSWORD_DEFAULT
            );

            $stmt = $conn->prepare(
                "INSERT INTO users (username, email, password)
                 VALUES (?, ?, ?)"
            );

            $stmt->bind_param(
                "sss",
                $username,
                $email,
                $hashedPassword
            );

            if ($stmt->execute()) {

                header("Location: login.php?signup=success");
                exit();

            } else {

                $message = "Pendaftaran gagal.";

            }
        }
    }
}

?>

<!DOCTYPE html>
<html>
<head>

    <title>Sign Up - Smart Food Waste Chopper</title>

    <link rel="stylesheet" href="style.css">

</head>

<body class="auth-page">

<div class="auth-box">

    <h1>SMART FOOD WASTE CHOPPER</h1>
    <h2>Create Account</h2>

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

        <label>Email</label>

        <input
            type="email"
            name="email"
            required
        >

        <label>Password</label>

        <input
            type="password"
            name="password"
            required
        >

        <button type="submit" class="primary-btn">
            SIGN UP
        </button>

    </form>

    <p>
        Already have an account?
        <a href="login.php">Login</a>
    </p>

</div>

</body>
</html>