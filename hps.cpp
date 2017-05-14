//***************************************************************************************
//Begin HPS Code
//***************************************************************************************

#include "cores/Arduino.h"
#include "common.h" //Common definitions for HPS CODE

//Variables for main loop (keep to a minimum)
double rpm_main = 0; //Store rpm 
double speed_main = 0; //Store speed
bool dms_triggered = false; //Store dead man switch status
double bat_voltage = 0; //Store battery voltage
Direction button_pressed; //Store d-pad button status
Mode race_mode; //Store Arduino mode


void setup() {
	setup_battery_monitor(); 
	datalogger_setup();
	rpm_sensor_setup();
	button_pressed = NONE;
	race_mode = standby; //Start submarine in standby mode
}

void loop() {
	button_pressed = dpad_getDirection(); //Check to see if a button was pressed each loop
	
	if (button_pressed == NONE) { //If a button is pressed, skip sensor readings and go straight to handling button input
		rpm_main = rpm_calculate(); //Calculate rpm
		dms_triggered = checkDMS(); //Check if dead man switch was triggered
		bat_voltage = measure_battery(); //Check battery level, mainly for datalogging
		//speed_main = speed_calculate(); //Calculate speed of sub
	}

	switch (race_mode) {
		case standby:
			if (button_pressed == CENTER) { // If center button pressed in standby, 
				race_mode = initialize; 	// move to initialization mode
			}
			break;
			
		case initialize:
			getBaseline(); //Initialize accelerometer
			//LCD display -> calibrating accelerometer
			//Takes ~30 seconds
			//LCD display -> Press LEFT to enter manual mode, RIGHT to enter autopilot mode
			do {
				button_pressed = dpad_getDirection(); //Wait for user input
				delay(50); 
			} while (button_pressed != LEFT || button_pressed != RIGHT); //Once LEFT or RIGHT is pressed, exit loop. 
				if (button_pressed == LEFT) {
					race_mode = manual;
					//LCD display -> entering manual control mode
				} else if (button_pressed == RIGHT) {
					race_mode = autopilot;
					//LCD display -> entering auto control mode
				}
			break;
			
		case autopilot:
			runAutoControls();
			break;
			
		case manual:
			runManualControls();
			break;
			
		case menu:
			if (button_pressed == DOWN) {
				//Move selection on menu down one
			} else if (button_pressed == UP) {
				//Move selection on menu up one
			} else if (button_pressed == CENTER) {
				//Make selection
			} else {
				//LCD display -> flash or do something to indicate invalid button press
			}
			break;
			
	} /* End Switch Statement */
	
	//Log raw rpm, pressure sensor readings
	//Log rpm, speed, bat voltage, dms variable values
}
