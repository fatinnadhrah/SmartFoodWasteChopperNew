
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <ESP32Servo.h>

// =========================
// PIN
// =========================
#define LIMIT_SWITCH_PIN 12
#define BUTTON_PIN 25
#define SERVO_PIN 26
#define BUZZER_PIN 18

#define RPWM 32
#define LPWM 33
#define R_EN 16
#define L_EN 17

// =========================
// LCD & SERVO
// =========================
hd44780_I2Cexp lcd;
Servo lockServo;

// SG90 SERVO
#define SERVO_LOCK_ANGLE 90
#define SERVO_UNLOCK_ANGLE 0

// =========================
// MOTOR
// =========================
#define MOTOR_SPEED 128
#define MOTOR_TIME 30000

bool motorRunning = false;
unsigned long motorStartTime = 0;

// =========================
// BUZZER
// =========================
#define BUZZER_TIME 6000

bool buzzerRunning = false;
unsigned long buzzerStartTime = 0;

// =========================
// SYSTEM STATE
// =========================
enum SystemState {
  READY,
  CHOPPING,
  COMPLETE,
  ERROR_STATE,
  LOCKING
};

SystemState systemState = READY;

// =========================
// WIFI
// =========================
const char* WIFI_SSID = "moonzzzz";
const char* WIFI_PASSWORD = "123456789";

void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {

  Serial.print("WiFi disconnect reason code: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  // Common ones:
  // 2   = AUTH_EXPIRE
  // 15  = 4WAY_HANDSHAKE_TIMEOUT  (classic WRONG PASSWORD signature)
  // 201 = NO_AP_FOUND
  // 202 = AUTH_FAIL              (wrong password)
  // 203 = ASSOC_FAIL
}

// IP address of the PC running XAMPP on the SAME Wi-Fi network as the ESP32.
// Find it with `ipconfig` on the PC (look under the "Wi-Fi" adapter, NOT
// any VMware/VirtualBox virtual adapter) and update this if the PC's IP changes.
const char* SAVE_DATA_URL =
  "http://172.20.10.2/SmartFoodWasteChopperNew/save_data.php";

// =========================
// LCD MEMORY
// =========================
String lastLine1 = "";
String lastLine2 = "";
String lastLine3 = "";
String lastLine4 = "";

// =========================
// LIMIT SWITCH
// HIGH = CLOSED
// LOW  = OPEN
// =========================
bool isLidClosed() {
  return digitalRead(LIMIT_SWITCH_PIN) == HIGH;
}

bool isLidOpen() {
  return digitalRead(LIMIT_SWITCH_PIN) == LOW;
}

// =========================
// CHECK LID SAFETY
// =========================
bool lidClosedSecure() {

  int closedCount = 0;

  Serial.println("Checking lid safety...");

  for (int i = 0; i < 3; i++) {

    if (isLidClosed()) {
      closedCount++;
    }

    delay(50);
  }

  Serial.print("Closed readings: ");
  Serial.print(closedCount);
  Serial.println("/3");

  if (closedCount >= 2) {

    Serial.println("LID SAFETY: OK");

    return true;
  }

  Serial.println("LID SAFETY: FAILED");

  return false;
}

// =========================
// MOTOR STOP
// =========================
void motorStop() {

  ledcWrite(RPWM, 0);
  ledcWrite(LPWM, 0);

  digitalWrite(R_EN, LOW);
  digitalWrite(L_EN, LOW);

  motorRunning = false;

  Serial.println("MOTOR: OFF");
}

// =========================
// MOTOR FORWARD
// =========================
void motorForward() {

  Serial.println("Preparing motor...");

  ledcWrite(RPWM, 0);
  ledcWrite(LPWM, 0);

  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);

  delay(200);

  ledcWrite(LPWM, 0);
  ledcWrite(RPWM, MOTOR_SPEED);

  motorRunning = true;

  Serial.println("MOTOR: ON");
}

// =========================
// SG90 LOCK
// =========================
void lockLid() {

  Serial.println();
  Serial.println("--------------------------");
  Serial.println("LOCKING LID...");
  Serial.println("--------------------------");

  // SG90 -> LOCK
  lockServo.write(SERVO_LOCK_ANGLE);

  Serial.println("SG90: MOVING TO LOCK");

  delay(1000);

  Serial.println("SG90: LOCK POSITION REACHED");
}

// =========================
// SG90 UNLOCK
// =========================
void unlockLid() {

  Serial.println();
  Serial.println("UNLOCKING LID...");

  // SG90 -> UNLOCK
  lockServo.write(SERVO_UNLOCK_ANGLE);

  delay(1000);

  Serial.println("SG90: UNLOCKED");
}

// =========================
// BUZZER OFF
// =========================
void buzzerOff() {

  digitalWrite(BUZZER_PIN, HIGH);

  buzzerRunning = false;

  Serial.println("BUZZER: OFF");
}

// =========================
// BUZZER COMPLETE
// =========================
void buzzerComplete() {

  digitalWrite(BUZZER_PIN, LOW);

  buzzerRunning = true;

  buzzerStartTime = millis();

  Serial.println("BUZZER: ON");
}

// =========================
// LCD UPDATE
// =========================
void updateLCD(
  String line1,
  String line2,
  String line3,
  String line4
) {

  if (line1 != lastLine1) {

    lcd.setCursor(0, 0);
    lcd.print("                    ");

    lcd.setCursor(0, 0);
    lcd.print(line1);

    lastLine1 = line1;
  }

  if (line2 != lastLine2) {

    lcd.setCursor(0, 1);
    lcd.print("                    ");

    lcd.setCursor(0, 1);
    lcd.print(line2);

    lastLine2 = line2;
  }

  if (line3 != lastLine3) {

    lcd.setCursor(0, 2);
    lcd.print("                    ");

    lcd.setCursor(0, 2);
    lcd.print(line3);

    lastLine3 = line3;
  }

  if (line4 != lastLine4) {

    lcd.setCursor(0, 3);
    lcd.print("                    ");

    lcd.setCursor(0, 3);
    lcd.print(line4);

    lastLine4 = line4;
  }
}

// =========================
// LCD DISPLAY
// =========================
void paparanLCD() {

  String machineStatus;
  String lidStatus;
  String motorStatus;

  if (systemState == READY)
    machineStatus = "READY";

  else if (systemState == CHOPPING)
    machineStatus = "PROCESSING";

  else if (systemState == COMPLETE)
    machineStatus = "COMPLETE";

  else if (systemState == LOCKING)
    machineStatus = "LOCKING";

  else
    machineStatus = "ERROR";

  if (isLidClosed())
    lidStatus = "CLOSED";

  else
    lidStatus = "OPEN";

  if (motorRunning)
    motorStatus = "ON";

  else
    motorStatus = "OFF";

  updateLCD(
    "MACHINE: " + machineStatus,
    "DETECTION: N/A",
    "LID: " + lidStatus,
    "MOTOR: " + motorStatus
  );
}

// =========================
// START MOTOR PROCESS
// =========================
void motorStart() {

  Serial.println();
  Serial.println("==========================");
  Serial.println("START PROCESS");
  Serial.println("==========================");

  // CHECK LID BEFORE LOCK
  if (!isLidClosed()) {

    Serial.println("ERROR: LID IS OPEN");
    Serial.println("MOTOR WILL NOT START");

    motorStop();

    systemState = ERROR_STATE;

    paparanLCD();

    return;
  }

  Serial.println("LID BEFORE LOCK: CLOSED");

  // =====================
  // LOCKING
  // =====================
  systemState = LOCKING;

  updateLCD(
    "MACHINE: LOCKING",
    "DETECTION: N/A",
    "LID: CLOSED",
    "MOTOR: OFF"
  );

  // SG90 LOCK
  lockLid();

  // =====================
  // VERIFY LOCK
  // =====================
  Serial.println();
  Serial.println("VERIFYING LOCK...");

  if (!lidClosedSecure()) {

    Serial.println("!!! LOCK FAILED !!!");
    Serial.println("MOTOR WILL NOT START");

    motorStop();

    unlockLid();

    systemState = ERROR_STATE;

    paparanLCD();

    return;
  }

  // FINAL LID CHECK
  if (!isLidClosed()) {

    Serial.println("!!! LID NOT CLOSED !!!");
    Serial.println("MOTOR WILL NOT START");

    motorStop();

    unlockLid();

    systemState = ERROR_STATE;

    paparanLCD();

    return;
  }

  // =====================
  // LOCK VERIFIED
  // =====================
  Serial.println();
  Serial.println("==========================");
  Serial.println("LOCK VERIFIED");
  Serial.println("STARTING MOTOR");
  Serial.println("==========================");

  // FINAL SAFETY CHECK
  if (!isLidClosed()) {

    Serial.println("FINAL SAFETY CHECK FAILED!");
    Serial.println("MOTOR BLOCKED!");

    motorStop();

    unlockLid();

    systemState = ERROR_STATE;

    paparanLCD();

    return;
  }

  // =====================
  // START PROCESSING
  // =====================
  systemState = CHOPPING;

  motorForward();

  motorStartTime = millis();

  paparanLCD();
}

// =========================
// MOTOR UPDATE
// =========================
void motorUpdate() {

  if (!motorRunning)
    return;

  // SAFETY: LID OPEN
  if (isLidOpen()) {

    Serial.println();
    Serial.println("!!! SAFETY !!!");
    Serial.println("LID OPEN!");
    Serial.println("MOTOR EMERGENCY STOP!");

    motorStop();

    unlockLid();

    systemState = ERROR_STATE;

    paparanLCD();

    return;
  }

  unsigned long elapsedTime =
    millis() - motorStartTime;

  unsigned long remainingTime = 0;

  if (elapsedTime < MOTOR_TIME) {

    remainingTime =
      (MOTOR_TIME - elapsedTime) / 1000;
  }

  // =====================
  // PROCESSING
  // =====================
  if (elapsedTime < MOTOR_TIME) {

    Serial.print("PROCESSING: ");
    Serial.print(remainingTime);
    Serial.println(" seconds");

    updateLCD(
      "MACHINE: PROCESSING",
      "DETECTION: N/A",
      "LID: CLOSED",
      "TIME: " + String(remainingTime) + "s"
    );
  }

  // =====================
  // COMPLETE
  // =====================
  if (elapsedTime >= MOTOR_TIME) {

    Serial.println();
    Serial.println("==========================");
    Serial.println("PROCESS COMPLETE");
    Serial.println("==========================");

    motorStop();

    delay(500);

    // SG90 UNLOCK
    unlockLid();

    systemState = COMPLETE;

    buzzerComplete();

    updateLCD(
      "MACHINE: COMPLETE",
      "DETECTION: N/A",
      "LID: OPEN",
      "MOTOR: OFF"
    );

    sendStatusToServer();
  }
}

// =========================
// SEND DATA TO SERVER
// =========================
void sendStatusToServer() {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi not connected.");

    return;
  }

  HTTPClient http;

  http.begin(SAVE_DATA_URL);

  http.addHeader(
    "Content-Type",
    "application/x-www-form-urlencoded"
  );

  String machineStatus;

  if (systemState == READY)
    machineStatus = "READY";

  else if (systemState == CHOPPING)
    machineStatus = "PROCESSING";

  else if (systemState == COMPLETE)
    machineStatus = "COMPLETE";

  else if (systemState == LOCKING)
    machineStatus = "LOCKING";

  else
    machineStatus = "ERROR";

  String lidStatus =
    isLidClosed() ? "CLOSED" : "OPEN";

  String motorStatus =
    motorRunning ? "ON" : "OFF";

  String data =
    "machine_status=" + machineStatus +
    "&detection=N/A" +
    "&lid_status=" + lidStatus +
    "&motor_status=" + motorStatus;

  Serial.println("Sending data:");
  Serial.println(data);

  int responseCode =
    http.POST(data);

  Serial.print("Server response: ");
  Serial.println(responseCode);

  http.end();
}

// =========================
// SETUP
// =========================
void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("==========================");
  Serial.println("SMART FOOD WASTE CHOPPER");
  Serial.println("ESP32 CORE V3");
  Serial.println("SG90 SERVO");
  Serial.println("==========================");

  // =====================
  // PIN MODE
  // =====================
  pinMode(
    LIMIT_SWITCH_PIN,
    INPUT_PULLUP
  );

  pinMode(
    BUTTON_PIN,
    INPUT_PULLUP
  );

  pinMode(
    BUZZER_PIN,
    OUTPUT
  );

  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  // =====================
  // DEFAULT OUTPUT
  // =====================
  digitalWrite(
    BUZZER_PIN,
    HIGH
  );

  digitalWrite(R_EN, LOW);
  digitalWrite(L_EN, LOW);

  // =====================
  // MOTOR PWM
  // ESP32 CORE V3
  // =====================
  Serial.println("Initializing motor PWM...");

  ledcAttach(
    RPWM,
    25000,
    8
  );

  ledcAttach(
    LPWM,
    25000,
    8
  );

  ledcWrite(RPWM, 0);
  ledcWrite(LPWM, 0);

  Serial.println("Motor PWM ready.");

  // =====================
  // SG90 SERVO
  // =====================
  Serial.println("Initializing SG90 servo...");

  lockServo.setPeriodHertz(50);

  lockServo.attach(
    SERVO_PIN,
    500,
    2400
  );

  Serial.print("Servo attached: ");

  if (lockServo.attached())
    Serial.println("YES");

  else
    Serial.println("NO");

  // START WITH UNLOCK
  unlockLid();

  Serial.println("SG90 servo ready.");

  // =====================
  // LCD
  // =====================
  Serial.println("Initializing LCD...");

  Wire.begin(21, 22);

  int lcdStatus =
    lcd.begin(20, 4);

  if (lcdStatus != 0) {

    Serial.print("LCD ERROR CODE: ");
    Serial.println(lcdStatus);

  } else {

    lcd.backlight();
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("SMART FOOD WASTE");

    lcd.setCursor(0, 1);
    lcd.print("CHOPPER SYSTEM");

    lcd.setCursor(0, 2);
    lcd.print("ESP32 CORE V3");

    lcd.setCursor(0, 3);
    lcd.print("INITIALIZING...");

    delay(2000);

    lcd.clear();

    lastLine1 = "";
    lastLine2 = "";
    lastLine3 = "";
    lastLine4 = "";
  }

  // =====================
  // WIFI
  // =====================
  WiFi.onEvent(onWiFiDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(200);

  Serial.println();
  Serial.println("Scanning for WiFi networks...");

  int networksFound = WiFi.scanNetworks();

  if (networksFound == 0) {

    Serial.println("No networks found at all!");

  } else {

    for (int i = 0; i < networksFound; i++) {

      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (RSSI ");
      Serial.print(WiFi.RSSI(i));
      Serial.print(", ch ");
      Serial.print(WiFi.channel(i));
      Serial.println(")");
    }
  }

  Serial.println("Connecting WiFi...");

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  unsigned long wifiStart =
    millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - wifiStart < 10000
  ) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("WiFi CONNECTED.");

    Serial.print("IP Address: ");

    Serial.println(
      WiFi.localIP()
    );

  } else {

    Serial.println(
      "WiFi NOT CONNECTED."
    );

    Serial.print("WiFi.status() code: ");
    Serial.println(WiFi.status());
    // 1 = WL_NO_SSID_AVAIL (SSID not seen)
    // 4 = WL_CONNECT_FAILED (wrong password / auth rejected)
    // 6 = WL_DISCONNECTED

    Serial.println(
      "System continues without WiFi."
    );
  }

  // =====================
  // SYSTEM READY
  // =====================
  systemState = READY;

  paparanLCD();

  sendStatusToServer();

  // =====================
  // LIMIT SWITCH STATUS
  // =====================
  Serial.println();
  Serial.println("==========================");
  Serial.println("LIMIT SWITCH STATUS");
  Serial.println("==========================");

  Serial.print("GPIO12 = ");

  Serial.println(
    digitalRead(LIMIT_SWITCH_PIN)
  );

  if (isLidClosed())
    Serial.println("LID = CLOSED");

  else
    Serial.println("LID = OPEN");

  Serial.println();
  Serial.println("==========================");
  Serial.println("SYSTEM READY");
  Serial.println("==========================");
}

// =========================
// LOOP
// =========================
void loop() {

  // =====================
  // MOTOR UPDATE
  // =====================
  motorUpdate();

  // =====================
  // BUZZER TIMER
  // =====================
  if (buzzerRunning) {

    if (
      millis() - buzzerStartTime
      >= BUZZER_TIME
    ) {

      buzzerOff();

      if (systemState == COMPLETE) {

        updateLCD(
          "MACHINE: COMPLETE",
          "DETECTION: N/A",
          "LID: OPEN",
          "MOTOR: OFF"
        );
      }
    }
  }

  // =====================
  // ERROR STATE
  // =====================
  if (systemState == ERROR_STATE) {

    motorStop();

    if (isLidClosed()) {

      Serial.println("LID CLOSED.");
      Serial.println("SYSTEM READY.");

      systemState = READY;

      paparanLCD();

      sendStatusToServer();
    }

    delay(50);

    return;
  }

  // =====================
  // COMPLETE STATE
  // =====================
  if (systemState == COMPLETE) {

    if (isLidOpen()) {

      Serial.println();
      Serial.println("LID OPEN.");
      Serial.println("SYSTEM READY.");

      systemState = READY;

      paparanLCD();

      sendStatusToServer();
    }

    delay(50);

    return;
  }

  // =====================
  // LOCKING STATE
  // =====================
  if (systemState == LOCKING) {

    motorStop();

    delay(50);

    return;
  }

  // =====================
  // BUTTON
  // =====================
  if (digitalRead(BUTTON_PIN) == LOW) {

    delay(50);

    if (digitalRead(BUTTON_PIN) == LOW) {

      Serial.println();
      Serial.println("BUTTON PRESSED");

      if (systemState == READY) {

        if (isLidClosed()) {

          motorStart();

          sendStatusToServer();

        } else {

          Serial.println(
            "CANNOT START."
          );

          Serial.println(
            "LID IS OPEN."
          );

          motorStop();

          systemState =
            ERROR_STATE;

          paparanLCD();

          sendStatusToServer();
        }
      }

      // WAIT BUTTON RELEASE
      while (
        digitalRead(BUTTON_PIN) == LOW
      ) {

        delay(10);
      }
    }
  }

  // =====================
  // LIMIT SWITCH MONITOR
  // =====================
  static int previousLimitState = -1;

  int currentLimitState =
    digitalRead(LIMIT_SWITCH_PIN);

  if (
    currentLimitState !=
    previousLimitState
  ) {

    previousLimitState =
      currentLimitState;

    Serial.print("GPIO12 = ");

    Serial.print(
      currentLimitState
    );

    if (currentLimitState == HIGH)
      Serial.println(
        " -> LID CLOSED"
      );

    else
      Serial.println(
        " -> LID OPEN"
      );

    if (systemState == READY)
      paparanLCD();
  }

  delay(50);
}

