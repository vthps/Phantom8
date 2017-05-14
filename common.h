#ifndef COMMON_H
#define COMMON_H

//Custom data type for button pad
enum Direction { NONE = 0, UP = 1, DOWN = 2, LEFT = 3, RIGHT = 4, CENTER = 5};

//Custom data type for mode
enum Mode { standby = 0, initialize = 1, autopilot = 2, manual = 3, menu = 4 };

//********************************************
//Pin assignments
//********************************************

#define PIN_RPM_INTERRUPT_1 18 
#define PIN_RPM_INTERRUPT_2 19
#define DEADMAN_SWITCH_PIN A2

#define pin_dpad 0
#define PitchServo1 22
#define PitchServo2 24
#define LRServo 26
#define LRServo2 28

//********************************************
// Function prototypes needed globally
//********************************************

//battery.cpp
void setup_battery_monitor();
double measure_battery();
bool checkDMS();

//datalogger.cpp
void datalogger_setup();

//controls.cpp
Direction dpad_getDirection();
void getBaseline();
void runAutoControls();
void runManualControls();
void cruise();

//rpm.cpp
void rpm_sensor_setup();
double rpm_calculate();

#endif /* COMMON_H */
