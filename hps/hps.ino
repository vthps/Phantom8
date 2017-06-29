//***************************************************************************************
//Begin HPS Code
//***************************************************************************************

#include "common.h" //Common definitions for HPS CODE
#include "actuator.cpp"
//#include "velocity.cpp"
//Variables for main loop (keep to a minimum)
double rpm_main = 0; //Store rpm 
double speed_main = 0; //Store speed
bool dms_triggered = false; //Store dead man switch status
double bat_voltage = 0; //Store battery voltage
Direction button_pressed; //Store d-pad button status
Mode race_mode; //Store Arduino mode

// VELOCITY VARIABLES 

double raw_static_pressure;
  double raw_dynamic_pressure;
  double static_voltage;
  double dynamic_voltage;
  double static_pressure;
  double dynamic_pressure;
  double velocity;

  double averVelo[200];
  int readIndex = 0;

// ACTUATOR VARIABLES

const int ACTUATOR_COUNT_THRESHOLD = 700;

const int ACTUATOR_POT_MIN = 223;
const int ACTUATOR_POT_MAX = 446;

const int ACTUATOR_LENGTH_MIN = 0;
const int ACTUATOR_LENGTH_MAX = 3;

const int ACTUATOR_TOLERANCE = 1;
const int ACTUATOR_STOP_TOL = 1;

// actuator pins
const int pin_act_pot = A8; //TODO: Put analog pin for actuator potentiometer // analog input pin
const int pin_act_pos = A11; //TODO: put digital out pin attached to pin 9 on H bridge // digital output pin
const int pin_act_gnd = 17; //TODO: put digital out pin attached to pin 1 on H bridge // digital output pin


// actuator variables
boolean actuator_target_hit = false;

int actuator_target = -1;
int actuator_perceived_position;
int actuator_hit_count = 0;

// VELOCITY FUNCTIONS


void get_velocity() {
  // this function can be called for in the race code and calculates velocity
  // the ftvelocity equation can be removed for the race code as it is for testing on the AOE tow tank
 
  //retrieve raw data
  raw_static_pressure = analogRead(static_sensor); // add/subtract constants to zero pressure sensors
  raw_dynamic_pressure = analogRead(dynamic_sensor);

  //convert from raw data to volts (5V = 1024 raw data increments)
  static_voltage = (raw_static_pressure/1024) * 5;
  dynamic_voltage = (raw_dynamic_pressure/1024) *5;

  //convert volts to Pascals (1V = 5500Pa)
  static_pressure = static_voltage * 5500; //Add Pa to these to zero the sensors to the same Pa if needed
  dynamic_pressure = dynamic_voltage * 5500;

  //calculated velocity in m/s using Bernoulli's Equation (see documentation)
  velocity = sqrt((2 * abs(static_pressure - dynamic_pressure)) / 1000);

  averVelo[readIndex++ % 200] = velocity;
  }

  

double averageVelocities() {
    double sum = 0;
    for (int i = 0; i < 200; i++) {
      sum += averVelo[i];
    }
    return sum / 200.0;
  }

void depth_function() {
  // this function can be called for in the race code and calculates depth

  //retrieve raw data
  double raw_static_pressure = analogRead(static_sensor);

  //convert from raw data to volts (5V = 1024 raw data increments)
  double static_voltage = (raw_static_pressure/1024) * 5;

  //convert volts to Pascals (1V = 5500Pa)
  double static_pressure = static_voltage * 5500;
  
  //calculated depth assuming 2988.98Pa/ft
  double depth = static_pressure / 2988.98;

}

// ACTUATOR FUNCTIONS



/**
 * DO NOT CALL THIS FUNCTION OUTSIDE OF OTHER ACTUATOR FUNCTIONS
 * Sets actuator to retract, extend,
 * or stay put, based on the difference
 * between the target value and the 
 * recorded value
 */
void actuator_set_pins(int difference) {

    //system processing to determine whether to stop the actuator or not
    if (abs(difference) <= ACTUATOR_STOP_TOL) {
        actuator_hit_count++;
        if(actuator_hit_count > ACTUATOR_COUNT_THRESHOLD) {
          actuator_target_hit = true;
          actuator_perceived_position = actuator_target;
        }
    }
    // stop actuator
    if (abs(difference) <= ACTUATOR_TOLERANCE && actuator_target_hit) {
        digitalWrite(pin_act_gnd, LOW);
        digitalWrite(pin_act_pos, LOW);
    }
    //retract actuator
    else if (difference < 0 && !actuator_target_hit) {
      digitalWrite(pin_act_gnd, LOW);
      digitalWrite(pin_act_pos, HIGH);
    }
    //extend actuator
    else if (difference > 0 && !actuator_target_hit) {
      Serial.println(difference);

      digitalWrite(pin_act_gnd, HIGH);
      digitalWrite(pin_act_pos, LOW);
    }
}


/*
 * Function to be called in setup() of the main 
 * program to setup actuator pins
 */
void actuator_setup() {
    pinMode(pin_act_gnd, OUTPUT);
    pinMode(pin_act_pos, OUTPUT);
}

/*
 * Function that NEEDS to be called at some point in 
 * loop() of the main program, to update the actuator
 * to ensure it does not go too far in either direction
 */
void actuator_loop() {
  int adc_val = analogRead(pin_act_pot);
  //Serial.println(adc_val);
  //Serial.println(actuator_target);
  if (actuator_target < 0) {
    actuator_target = adc_val;
    actuator_perceived_position = adc_val;
  }
  if (actuator_target - actuator_perceived_position != 0) {
    actuator_set_pins(actuator_target - adc_val);
  }
}




/*
 * Function called to stop the actuator in case of emergency
 */
void actuator_stop() {
  int adc_val = analogRead(pin_act_pot);
  actuator_target = adc_val;
  actuator_set_pins(actuator_target - adc_val);
}

/**
 * DO NOT CALL THIS FUNCTION OUTSIDE OF OTHER ACTUATOR FUNCTIONS
 * Returns target value on the potentiometera
 * for a given length (ONLY WORKS FOR WHICHEVER
 * ACTUATOR THE CODE IS CURRENTLY CALIBRATED FOR)
 * CURRENTLY THE WATERPROOFED 63:1 ACTUATOR MARKED (A)
 */
int actuator_target_value(float length) {
    actuator_target_hit = false;
    actuator_hit_count = 0;
    
    if (length < ACTUATOR_LENGTH_MIN) {
      return ACTUATOR_POT_MIN;
    }
    else if (length > ACTUATOR_LENGTH_MAX) {
      return ACTUATOR_POT_MAX;
    }
    else {            
      double val =  15.26*pow(length,3)-36.18*pow(length,2)+101.7*length+179.69; //calculated model for 
      if (val < ACTUATOR_POT_MIN) {                                     //63:1 actuator A
        return ACTUATOR_POT_MIN;
      }
      else if (val > ACTUATOR_POT_MAX) {
        return ACTUATOR_POT_MAX;
      }
      else {
        return val;
      }
    }
}

/*
 * Function called to move the actuator to a specified length
 * e.g. 0.5 to move to half an inch, 1.5 to move the an inch and a half
 */
void actuator_move_to(double actuator_length){
  actuator_target = actuator_target_value(actuator_length);
}
/*
 * Function called to move the actuator in an increment
 * e.g. +0.1 to extend a tenth of an inch, -0.1 to retract
 */
void actuator_move_increment(double actuator_distance_change) {
  actuator_move_to(actuator_target + actuator_distance_change);
}


void gotoOptimalLength(double avgVelo, int rpm)
{
   float angle =  atan(avgVelo*39.3701/(3.14*12*rpm/60)); //Setting it so that the midpoints of the blade are at the optimal angle because honestly I don't have the time to do this properly please oh please god can I sleep now
   float length = angle/56;
   actuator_move_to(length);
//   int target = target_value(length);
//   adc_val = analogRead(pin_pot);
//
//   if (target < 0) {
//    target = adc_val;
//  }
//  if (perceived_position == NULL){
//    perceived_position = adc_val;
//  }
//  if (target - perceived_position != 0) {
//      set_pins(target - adc_val);
// }
}


void setup() {
	setup_battery_monitor(); 
	//datalogger_setup();
	rpm_sensor_setup();
  actuator_setup();
	button_pressed = NONE;
	race_mode = standby; //Start submarine in standby mode
}

void loop() {
	button_pressed = dpad_getDirection(); //Check to see if a button was pressed each loop
	
	if (button_pressed == NONE) { //If a button is pressed, skip sensor readings and go straight to handling button input
		rpm_main = rpm_calculate(); //Calculate rpm
		dms_triggered = checkDMS(); //Check if dead man switch was triggered
		bat_voltage = measure_battery(); //Check battery level, mainly for datalogging
		get_velocity(); //Calculate speed of sub
   
	}

	switch (race_mode) {
		case standby:
			if (button_pressed == CENTER) { // If center button pressed in standby, 
				race_mode = initialize; 	// move to initialization mode
			}
			break;
			
		case initialize:
			//getBaseline(); //Initialize accelerometer
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
      gotoOptimalLength(averageVelocities(), rpm_main);
      actuator_loop();
			//runAutoControls();
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
