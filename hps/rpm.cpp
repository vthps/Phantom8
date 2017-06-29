#include "Arduino.h"
#include "common.h"


/* This file contains functions that will:
 * 1) Calculate RPM at any time 
 * 2) Setup the pin interrupts to allow said
 *    calculations to occur
 */
const unsigned int TIME_THRESHOLD = 5000; //Threshold time (milliseconds)
volatile int rpm_hit_count; //Global counter incremented in interrupt service routine

void rpm_count_increment(); //Function prototype rpm interrupt function

/*
 * Run this code at some point in the
 * setup method to initialize RPM sensor
 * interrupts
 */
void rpm_sensor_setup() {
    attachInterrupt(digitalPinToInterrupt(PIN_RPM_INTERRUPT_1), rpm_count_increment, RISING);
    attachInterrupt(digitalPinToInterrupt(PIN_RPM_INTERRUPT_2), rpm_count_increment, RISING);
}


/*
 * Run this code at some point in the loop
 * to calculate rpm since the last loop
 * 
 * for code convenience, please use:
 * 
 * rpm_current = rpm_calculate();
 */
double rpm_calculate() {
    static double rpm_current;
    static unsigned long rpm_time;
 
    if (millis() - rpm_time >= TIME_THRESHOLD) {
        unsigned long tmp_time = rpm_time;
        rpm_time = millis();
        double rpm = (((double) 60000* rpm_hit_count) / (rpm_time - tmp_time)) / 2; 
        rpm_hit_count = 0;         
        return rpm;
    } else {
        return rpm_current;
    }
}


/*
 * Function called only when rpm_interrupt occurs
 * DO NOT CALL THIS FROM ANY POINT IN THE CODE!!!
 */
void rpm_count_increment(){
    //Serial.print("hit "); //For testing only
    rpm_hit_count++;
}
