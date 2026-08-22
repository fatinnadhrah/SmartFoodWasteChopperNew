#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <ESP32Servo.h>

// ===============================
// SENSOR + BUZZER
// ===============================

#define TRIG_PIN 12
#define ECHO_PIN 14

#define BUZZER_PIN 18

// ===============================
// BTS7960 MOTOR
// ===============================

#define MOTOR_RPWM 32
#define MOTOR_LPWM 33

#define R_EN_PIN 16
#define L_EN_PIN 17

// Use different channels to avoid servo conflict

#define MOTOR_R_CHANNEL 2
#define MOTOR_L_CHANNEL 3


#define MOTOR_FREQ 25000
#define MOTOR_RESOLUTION 8


// ===============================
// BUTTON
// ===============================

#define BUTTON_PIN 25


bool lastButtonState = HIGH;


// ===============================
// MOTOR STATE
// ===============================

bool motorRunning = false;


unsigned long motorStartTime = 0;


const unsigned long MOTOR_RUN_TIME = 30000;


// ===============================
// BUTTON SERVO LOCK STATE
// ===============================

bool buttonLockActive = false;


unsigned long buttonLockStartTime = 0;


const unsigned long BUTTON_LOCK_TIME = 30000;

// ===============================
// SERVO
// ===============================

Servo myServoPenutup;

Servo myServoLocker;

// ===============================
// LCD
// ===============================

hd44780_I2Cexp lcd;

// ===============================
// SYSTEM STATE
// ===============================

bool processComplete = false;

// ===============================
// BTS7960 MOTOR STOP
// ===============================

void motorStop()
{


  ledcWriteChannel(
    MOTOR_R_CHANNEL,
    0
  );


  ledcWriteChannel(
    MOTOR_L_CHANNEL,
    0
  );



  digitalWrite(
    R_EN_PIN,
    LOW
  );


  digitalWrite(
    L_EN_PIN,
    LOW
  );



  Serial.println("MOTOR STOP");

}

// ===============================
// BTS7960 MOTOR FORWARD
// ===============================

void motorForward(int speed)
{


  digitalWrite(
    R_EN_PIN,
    HIGH
  );


  digitalWrite(
    L_EN_PIN,
    HIGH
  );



  int pwmValue = map(
    speed,
    0,
    100,
    0,
    255
  );



  ledcWriteChannel(
    MOTOR_R_CHANNEL,
    pwmValue
  );


  ledcWriteChannel(
    MOTOR_L_CHANNEL,
    0
  );


  Serial.print("PWM VALUE: ");

  Serial.println(pwmValue);


}

// ===============================
// MOTOR START
// ===============================

void motorFunction()
{

  if(!motorRunning)
  {


    Serial.println("MOTOR START");


    motorForward(50);



    motorRunning = true;


    motorStartTime = millis();


  }

}

// ===============================
// MOTOR TIMER
// ===============================

void motorUpdate()
{


  if(motorRunning)
  {


    if(millis() - motorStartTime >= MOTOR_RUN_TIME)
    {


      motorStop();


      motorRunning = false;



      Serial.println("MOTOR COMPLETE");


    }


  }


}
// ===============================
// BUTTON SERVO LOCK FUNCTION
// ===============================

void buttonLockFunction()
{

  Serial.println("BUTTON LOCK START");


  myServoPenutup.write(90);


  delay(500);


  myServoLocker.write(90);



  paparanLCD(
    "RUNNING",
    "NO",
    "LOCKED"
  );



  // Active LOW buzzer OFF

  digitalWrite(
    BUZZER_PIN,
    HIGH
  );



  buttonLockActive = true;


  buttonLockStartTime = millis();

}

// ===============================
// BUTTON COMPLETE FUNCTION
// ===============================

void buttonCompleteFunction()
{
  myServoPenutup.write(0);


  myServoLocker.write(0);

  Serial.println("BUTTON PROCESS COMPLETE");


  paparanLCD(
    "COMPLETE",
    "NO",
    "UNLOCKED"
  );


  buzzerComplete();
  delay(30000);
  
}


// ===============================
// BUTTON SERVO LOCK UPDATE
// ===============================

void buttonLockUpdate()
{


  if(buttonLockActive)
  {


    // Hold servo position

    myServoPenutup.write(90);

    myServoLocker.write(90);




    if(millis() - buttonLockStartTime >= BUTTON_LOCK_TIME)
    {


      buttonLockActive = false;



      Serial.println("BUTTON LOCK COMPLETE");



      buttonCompleteFunction();


    }

  }


}

// ===============================
// BUZZER COMPLETE
// ACTIVE LOW
// ===============================

void buzzerComplete()
{

  // BUZZER ON

  digitalWrite(
    BUZZER_PIN,
    LOW
  );



  delay(6000);



  // BUZZER OFF

  digitalWrite(
    BUZZER_PIN,
    HIGH
  );

}

// ===============================
// HC-SR04 DISTANCE
// ===============================

float bacaJarak()
{

  digitalWrite(
    TRIG_PIN,
    LOW
  );


  delayMicroseconds(2);



  digitalWrite(
    TRIG_PIN,
    HIGH
  );


  delayMicroseconds(10);



  digitalWrite(
    TRIG_PIN,
    LOW
  );



  long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    30000
  );



  if(duration == 0)
  {

    return -1;

  }



  return duration * 0.0343 / 2;

}

// ===============================
// LCD DISPLAY
// ===============================

void paparanLCD(
String machineStatus,
String detection,
String lid
)
{

  lcd.clear();



  lcd.setCursor(0,0);

  lcd.print("MACHINE: ");

  lcd.print(machineStatus);



  lcd.setCursor(0,1);

  lcd.print("DETECTION: ");

  lcd.print(detection);



  lcd.setCursor(0,2);

  lcd.print("LID: ");

  lcd.print(lid);


}
// ===============================
// READY STATE
// ===============================

void keadaanReady()
{

  myServoPenutup.write(90);


  myServoLocker.write(0);



  paparanLCD(
    "READY",
    "NO",
    "UNLOCKED"
  );



  // Active LOW buzzer OFF

  digitalWrite(
    BUZZER_PIN,
    HIGH
  );

}

// ===============================
// PROCESS STATE
// ===============================

void keadaanProcess()
{

  myServoPenutup.write(0);


  myServoLocker.write(0);



  paparanLCD(
    "PROCESS",
    "YES",
    "UNLOCKED"
  );



  // Active LOW buzzer OFF

  digitalWrite(
    BUZZER_PIN,
    HIGH
  );

}

// ===============================
// LOCKED STATE
// ===============================

void keadaanLocked()
{

  myServoPenutup.write(90);



  delay(500);



  myServoLocker.write(90);



  processComplete = true;



  paparanLCD(
    "Waiting...",
    "NO",
    "LOCKED"
  );

}

// ===============================
// SETUP
// ===============================

void setup()
{

  Serial.begin(115200);


  // ===============================
  // BTS7960
  // ===============================

  pinMode(
    R_EN_PIN,
    OUTPUT
  );


  pinMode(
    L_EN_PIN,
    OUTPUT
  );



  digitalWrite(
    R_EN_PIN,
    LOW
  );


  digitalWrite(
    L_EN_PIN,
    LOW
  );



  pinMode(
    MOTOR_RPWM,
    OUTPUT
  );


  pinMode(
    MOTOR_LPWM,
    OUTPUT
  );



  digitalWrite(
    MOTOR_RPWM,
    LOW
  );


  digitalWrite(
    MOTOR_LPWM,
    LOW
  );



  delay(500);

  // ===============================
  // SENSOR + BUZZER
  // ===============================

  pinMode(
    TRIG_PIN,
    OUTPUT
  );


  pinMode(
    ECHO_PIN,
    INPUT
  );



  pinMode(
    BUZZER_PIN,
    OUTPUT
  );


  // Active LOW buzzer OFF

  digitalWrite(
    BUZZER_PIN,
    HIGH
  );

  // ===============================
  // SERVO LEDC FIX
  // ===============================

  ESP32PWM::allocateTimer(3);

  myServoPenutup.setPeriodHertz(50);


  myServoPenutup.attach(
    27,
    500,
    2400
  );



  myServoLocker.setPeriodHertz(50);


  myServoLocker.attach(
    26,
    500,
    2400
  );



  myServoPenutup.write(90);


  myServoLocker.write(0);

  // ===============================
  // MOTOR PWM
  // ===============================

  ledcAttachChannel(
    MOTOR_RPWM,
    MOTOR_FREQ,
    MOTOR_RESOLUTION,
    MOTOR_R_CHANNEL
  );



  ledcAttachChannel(
    MOTOR_LPWM,
    MOTOR_FREQ,
    MOTOR_RESOLUTION,
    MOTOR_L_CHANNEL
  );



  delay(100);



  motorStop();

  // ===============================
  // BUTTON
  // ===============================

  pinMode(
    BUTTON_PIN,
    INPUT_PULLUP
  );

  // ===============================
  // LCD
  // ===============================

  Wire.begin(
    21,
    22
  );



  int status = lcd.begin(
    20,
    4
  );



  if(status)
  {

    Serial.print("LCD ERROR: ");

    Serial.println(status);


    while(1);

  }



  lcd.backlight();



  keadaanReady();



  Serial.println("SYSTEM READY");

  Serial.println("PRESS GPIO25 BUTTON");


}
// ===============================
// LOOP
// ===============================

void loop()
{


  // ===============================
  // UPDATE TIMER
  // ===============================

  motorUpdate();


  buttonLockUpdate();

  // ===============================
  // BUTTON PRESS + RELEASE
  // ===============================

  bool buttonState = digitalRead(
    BUTTON_PIN
  );



  if(lastButtonState == HIGH && buttonState == LOW)
  {


    delay(50);



    if(digitalRead(BUTTON_PIN) == LOW)
    {


      Serial.println("BUTTON PRESSED");



      // WAIT RELEASE

      while(digitalRead(BUTTON_PIN) == LOW)
      {

        delay(10);

      }



      Serial.println("BUTTON RELEASED");






      // KEEP OLD WORKING MOTOR FUNCTION

      motorFunction();






      // ADD SERVO LOCK FUNCTION

      buttonLockFunction();



    }

  }



  lastButtonState = buttonState;


  // ===============================
  // IMPORTANT
  // KEEP MOTOR RUNNING
  // ===============================

  if(motorRunning)
  {

    delay(10);

    return;

  }

  // ===============================
  // IGNORE ULTRASONIC DURING BUTTON LOCK
  // ===============================

  if(buttonLockActive)
  {

    delay(10);

    return;

  }


  // ===============================
  // ULTRASONIC CHECK
  // ===============================

  float jarak = bacaJarak();



  Serial.print("Distance: ");





  if(jarak < 0)
  {

    Serial.println("ERROR");

  }
  else
  {

    Serial.print(jarak,1);

    Serial.println(" cm");

  }


  // ===============================
  // NO OBJECT
  // ===============================

  if(jarak < 10 || jarak > 40)
  {


    keadaanReady();


    delay(500);


  }

  // ===============================
  // OBJECT DETECTED
  // ===============================

  else
  {


    processComplete = false;



    keadaanProcess();



    Serial.println("OBJECT DETECTED");



    unsigned long startTime = millis();


    while(millis() - startTime < 20000)
    {

      delay(10);

    }


    float checkJarak = bacaJarak();


    if(checkJarak > 10 && checkJarak <= 40)
    {


      keadaanProcess();


      Serial.println("OBJECT STILL THERE");


    }
    else
    {


      keadaanLocked();


      Serial.println("PROCESS COMPLETE");



      // Active LOW buzzer OFF

      digitalWrite(
        BUZZER_PIN,
        HIGH
      );


    }



    delay(5000);


  }


}
