#include "Arduino.h"
#include "common.h"

  

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
