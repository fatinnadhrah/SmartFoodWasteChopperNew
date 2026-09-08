
#include <WiFi.h>
#include <WebServer.h>

// ==================================================
// 1. WIFI SETTING
// ==================================================

const char* ssid = "NAMA_WIFI_KAU";
const char* password = "PASSWORD_WIFI_KAU";


// ==================================================
// 2. ULTRASONIC SENSOR
// ==================================================

const int TRIG_PIN = 5;
const int ECHO_PIN = 18;


// ==================================================
// 3. SENSOR STATUS
// ==================================================

bool sensorStatus = false;


// ==================================================
// 4. WEB SERVER
// ==================================================

WebServer server(80);


// ==================================================
// 5. BACA JARAK ULTRASONIC
// ==================================================

float getDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  float distance = duration * 0.0343 / 2;

  return distance;
}


// ==================================================
// 6. SENSOR ON
// ==================================================

void sensorOn() {

  sensorStatus = true;

  Serial.println("Sensor ON");

  server.send(
    200,
    "text/plain",
    "Sensor ON"
  );
}


// ==================================================
// 7. SENSOR OFF
// ==================================================

void sensorOff() {

  sensorStatus = false;

  Serial.println("Sensor OFF");

  server.send(
    200,
    "text/plain",
    "Sensor OFF"
  );
}


// ==================================================
// 8. SENSOR STATUS
// ==================================================

void sensorStatusPage() {

  if (sensorStatus == true) {

    server.send(
      200,
      "text/plain",
      "ON"
    );

  } else {

    server.send(
      200,
      "text/plain",
      "OFF"
    );
  }
}


// ==================================================
// 9. READ DISTANCE
// ==================================================

void readSensor() {

  if (sensorStatus == false) {

    server.send(
      200,
      "text/plain",
      "Sensor OFF"
    );

    return;
  }

  float distance = getDistance();

  if (distance < 0) {

    server.send(
      200,
      "text/plain",
      "No reading"
    );

    return;
  }

  String result = String(distance, 2) + " cm";

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  server.send(
    200,
    "text/plain",
    result
  );
}


// ==================================================
// 10. SETUP
// ==================================================

void setup() {

  Serial.begin(115200);


  // ------------------------------------------------
  // Ultrasonic PIN
  // ------------------------------------------------

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);


  // ------------------------------------------------
  // CONNECT WIFI
  // ------------------------------------------------

  Serial.println();
  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }


  // ------------------------------------------------
  // WIFI CONNECTED
  // ------------------------------------------------

  Serial.println();
  Serial.println("WiFi Connected!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());


  // ------------------------------------------------
  // WEBSITE COMMAND
  // ------------------------------------------------

  server.on(
    "/sensor/on",
    HTTP_GET,
    sensorOn
  );


  server.on(
    "/sensor/off",
    HTTP_GET,
    sensorOff
  );


  server.on(
    "/sensor/status",
    HTTP_GET,
    sensorStatusPage
  );


  server.on(
    "/sensor/read",
    HTTP_GET,
    readSensor
  );


  // ------------------------------------------------
  // START SERVER
  // ------------------------------------------------

  server.begin();

  Serial.println("ESP32 Web Server Started!");
}


// ==================================================
// 11. LOOP
// ==================================================

void loop() {

  // Check website commands
  server.handleClient();


  // ------------------------------------------------
  // READ SENSOR ONLY WHEN ON
  // ------------------------------------------------

  if (sensorStatus == true) {

    float distance = getDistance();

    if (distance >= 0) {

      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");

    } else {

      Serial.println("No ultrasonic reading");
    }

    delay(500);
  }
}

