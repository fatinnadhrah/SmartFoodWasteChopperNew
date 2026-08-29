 Smart Food Waste Chopper
 
1. Project Overview
Smart Food Waste Chopper is an IoT-based food waste management system designed to detect, process and manage food waste automatically. The system uses an ESP32 as the main controller and integrates sensors, servo motors, a DC motor, OLED display and buzzer. The system also provides a web-based monitoring interface that allows users to monitor the machine status in real time.

2. Problem Statement
Food waste is commonly disposed of directly into normal waste bins without being processed. This can cause unpleasant odors, attract insects and increase the volume of waste. Manual handling of food waste can also be inconvenient and may require users to come into direct contact with the waste.

Therefore, the Smart Food Waste Chopper is developed to automate the food waste detection and chopping process while providing a safer and more efficient waste management method.

3. Project Objectives
The objectives of this project are:

- To detect food waste using an ultrasonic sensor.
- To automatically open and close the lid using a servo motor.
- To lock the lid before the chopping process starts.
- To chop food waste using a DC motor and chopping blade.
- To display the machine status using an OLED display.
- To monitor the machine through a web-based interface.
- To store machine status data in a MySQL database.
- To provide an alert using a buzzer when the chopping process is completed.

4. Main System Functions
   
4.1 Food Waste Detection
The HC-SR04 ultrasonic sensor is used to detect the presence of food waste inside the container. When food waste is detected, the system changes the detection status to `YES`.

4.2 Automatic Lid Control
A servo motor is used to control the automatic opening and closing of the lid.

The lid can:
- Open when food waste is detected.
- Remain open while the user inserts food waste.
- Close after the locking countdown.
- Open again after the chopping process is completed.

4.3 Lid Locking
After the food waste is detected, the system starts a 30-second countdown. During the countdown, the user can add additional food waste.

After the countdown reaches zero:
1. The lid closes.
2. The lid lock servo locks the lid.
3. The machine enters processing mode.
4. The chopping motor starts.
The chopping motor will only operate after the lid is locked.

4.4 Chopping Process
The DC motor drives the chopping blade to process the food waste. The chopping process runs for the programmed duration.

During the process, the system displays:
Machine: PROCESS
Detection: YES
Lid: LOCKED
Motor: ON
The remaining processing time is also displayed on the website.

4.5 DONE Function
The website provides a DONE button for the user. The user presses DONE after finishing the insertion of food waste.

When the DONE command is received:
1. The system stops the food insertion stage.
2. The lid closes.
3. The lid is locked.
4. The chopping process begins.
This allows the user to manually indicate that no more food waste will be added.

4.6 Additional Chopping
- The website provides a CHOPPING button.
- If the food waste requires additional processing, the user can press the CHOPPING button to continue the chopping operation.

5. OLED Display
The OLED display provides information about the current machine condition.

The display can show:
Machine Status
Detection Status
Lid Status
Remaining Time

Ready State:
MACHINE: READY
DETECTION: NO
LID: UNLOCKED

Detection State:
MACHINE: READY
DETECTION: YES
LID: UNLOCKED

Processing State:
MACHINE: PROCESS
DETECTION: YES
LID: LOCKED

Completed State:
MACHINE: COMPLETED
DETECTION: YES
LID: UNLOCKED

6. Web-Based Monitoring System
The project includes a web-based monitoring system.

The website allows users to monitor:
- Machine status
- Food detection status
- Lid status
- Motor status
- Remaining processing time
- Process completion
- System commands

The website is developed using:
- HTML
- CSS
- JavaScript
- PHP

7. IoT Communication
- The ESP32 connects to a Wi-Fi network and communicates with the web server.
- The ESP32 sends machine status data to the PHP server.

The communication flow is:
ESP32
  ↓
Wi-Fi
  ↓
PHP
  ↓
MySQL
  ↓
Website
- This allows the website to display the current status of the physical machine.

8. Database
- The project uses MySQL to store machine status information.
Database Name: smart_food_waste_chopper

9. Hardware Component
   ESP32: Main microcontroller and Wi-Fi communication
   HC-SR04 ultrasonic sensor: Detects food waste
   MG996R servo motor: Controls the automatic lid
   servo motor: lock & unlock the lid
   DC Motor 775: Drives the chopping blade
   BTS7960 Motor Driver: Controls the DC motor
   OLED Display: Displays machine status
   Buzzer: Provides an alert when the process is completed
   12V Power Supply: Supplies power to the system
   ON/OFF Switch: Turns the system on and off
   Chopping Blade: Chops food waste
   Dustbin Container: Holds food waste before chopping
   Drawer/Container: Collects processed food waste

10. Software requirements
    - Arduino IDE
    - Visual Studio Code
    - XAMPP
    - MySQL
    - phpMyAdmin

11. system process flow
START
  ↓
Machine READY
  ↓
Sensor detects food waste
  ↓
Detection = YES
  ↓
Lid = UNLOCKED
  ↓
User inserts food waste
  ↓
30-second countdown
  ↓
User can add additional food waste
  ↓
User presses DONE
  ↓
Lid closes
  ↓
Lid LOCKED
  ↓
Chopping process starts
  ↓
Motor ON
  ↓
Chopping countdown
  ↓
Is additional chopping required?
  │
  ├── YES → Press CHOPPING
  │           ↓
  │       Continue chopping
  │
  └── NO
       ↓
    Motor OFF
       ↓
    Lid UNLOCKED
       ↓
    Lid opens
       ↓
    Buzzer ON for 6 seconds
       ↓
    COMPLETED
       ↓
    Processed food waste collected
       ↓
    System returns to READY

12. Food Waste Processing Area
- The container is designed with aluminium sheets on the left and right sides of the chopping chamber.
- The aluminium sheets act as a boundary for the food waste processing area.
- The food waste is directed into the designated chopping chamber so that it remains within the controlled chopping area.
- The processed food waste is then directed towards the lower collection drawer.
- The motor and electrical components are positioned away from the direct food waste path to reduce the risk of food waste entering the motor and electrical components.

status projek masih dalam pembinaan coding untuk button switch on/off, menyusun komponen-komponen pada tong & masih dalam proses connect esp dengan wifi.
   
