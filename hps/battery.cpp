#include "Arduino.h"
#include "common.h"

#define BATTERY_ANALOG_PIN A0 //Pin for battery voltage measurement
#define BATTERY_TOP_RESISTOR 1000000 //Top Resistor is 1 MOhm
#define BATTERY_BOTTOM_RESISTOR 390000 //Bottom Resistor is 390 kOhm
#define BATTERY_LOW_VOLTAGE 13.5 //below 13.5v the 12v regulator will likely not work
#define BATTERY_HIGH_VOLTAGE 16.8 //4.2 volts * 4s

void setup_battery_monitor() {
    pinMode(BATTERY_ANALOG_PIN, INPUT);
    pinMode(DEADMAN_SWITCH_PIN, INPUT);
}

double measure_battery() {
    double rawRead = analogRead(BATTERY_ANALOG_PIN); //Returns value between 0 and 1024;
    rawRead = rawRead * 5 / 1024; //Convert to real voltage
    double correctedVoltage = (rawRead * (BATTERY_TOP_RESISTOR + BATTERY_BOTTOM_RESISTOR)) / BATTERY_BOTTOM_RESISTOR; //Compensate for voltage divider
    return correctedVoltage;
}

bool checkDMS() {
	if (digitalRead(DEADMAN_SWITCH_PIN) == HIGH)
		return true;
	else
		return false;
}
