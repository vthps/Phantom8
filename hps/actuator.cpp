#include "Arduino.h"
#include "common.h"
#include "actuator.h"

// actuator constants
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

// variable pitch constants

int topSpeed = 3;




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


void gotoOptimalLength(double avgVelo)
{
  float angle = 5;
   if (avgVelo > topSpeed)
   {
    angle = 45;
   } else if (avgVelo > 0) {   
    float angle = angle + (40*avgVelo/topSpeed);
   }
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
