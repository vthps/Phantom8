/* NEED TO DO: fill in correct pin numbers and conversion factors
 * 10/12/16 
 * 
 * This file creates a function that takes in pressure measurements
 * and implements the velocity calculation 
 * 
 * (from handwritten page in documentation)
 * 
 * Bernoulli's Equation:
 * Stagnation_pressure = static_pressure + dynamice_pressure
 * 
 * Pt = Ps + (rho*v^2)/2
 * v = sqrt(2*(Pt-Ps)/rho)
 * 
 * where:
 * Pt = stagnation_pressure [Pa]
 * Ps = static_pressure [Pa]
 * rho = fluid density (of water) 1000kg/m^3
 * v = velocity [m/s]
 */

//pins (NEED TO UPDATE PINS NUMBERS)
int pin_pressure_static = 0;
int pin_pressure_stagnation = 0;

//variables
double stagnation_pressure_raw;    //output signal
double stagnation_pressure;        //PSI
double static_pressure_raw;        //output signal
double static_pressure;            //PSI
double velocity;                   //m/s
double fluid_density_water = 1000; //kg/m^3

void setup() {
  // no setup needed

  Serial.begin(9600);
}

void loop() {
  calc_velocity();

  //print to serial window
  Serial.print(stagnation_pressure_raw);
  Serial.print("stagnation (volts):");
  Serial.println(stagnation_pressure);
  Serial.print("static (raw):");
  Serial.print(static_pressure_raw);
  Serial.print("static (volts):");
  Serial.print(static_pressure);
  Serial.print("stagnation (raw):");
}

void calc_velocity(){
  //fetch data
  stagnation_pressure_raw = analogRead(pin_pressure_stagnation);
  static_pressure_raw = analogRead(pin_pressure_static);

  //convert raw to PSI (NEED TO UPDATE W CORRECT CONVERSION FACTOR)
  pressure_static = pressure_static_raw; 
  pressure_stagnation = pressure_stagnation_raw;

  //calculate velocity 
  //(Bernoulli's Equation, see comments above)
  velocity = sqrt(2*abs(stagnation_pressure-static_pressure)/fluid_density_water);
}

