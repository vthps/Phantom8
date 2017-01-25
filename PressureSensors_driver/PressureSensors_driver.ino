
/*
 * Brooke Taylor
 * bt2016@vt.edu
 * 04/30/16
 *
 * This file will read the values from the pressure sensors and
 * convert to PSI. The values will be printed to the serial window.
 *
 * Key Hardware Components:
 *     2x Keller America 0-5V output Pressure Sensors
 *     Arduino Mega 2560
 * 
 *     Static Sensor:
 *     Pin 1 - Shield +12V
 *     Pin 2 - A3
 *     Pin 3 -
 *     Pin 4 - Shield GND
 *
 *     Dynamic Sensor:
 *     Pin 1 - Shield +12V
 *     Pin 2 - A4
 *     Pin 3 -
 *     Pin 4 - Shield GND
 * 
 */


//pins
int pin_pressure_static = A3;
int pin_pressure_stagnation = A4;

//variables
double pressure_static_raw;     //ADC steps
double pressure_stagnation_raw; //ADC steps
double pressure_static;         //psi
double pressure_stagnation;     //psi
double pressure_velocity;       //meters per second
double pressure_depth;          //depth of the static sensor (meters)




void setup() {
    //no setup required for pressure sensors
  
    Serial.begin(9600);
}

void loop() {
    pressure_loop();

    //print to serial window
    Serial.print("static_raw:");
    Serial.print(pressure_static_raw);
    Serial.print(" static_volts:");
    Serial.print(pressure_static);
    Serial.print(" dynamic_raw:");
    Serial.print(pressure_stagnation_raw);
    Serial.print(" dynamic_volts:");
    Serial.println(pressure_stagnation);
}


void pressure_loop(){
    //retrieve data
    pressure_static_raw = analogRead(pin_pressure_static);
    pressure_stagnation_raw = analogRead(pin_pressure_stagnation);

    //convert from raw to PSI (5V=1024steps and 1V=6psi)
    pressure_static = ((pressure_static_raw/1024) * 5 * 6) + 14.7 ;
    pressure_stagnation = ((pressure_stagnation_raw/1024) * 5 * 6) + 14.7;
  
    //calculate depth assuming that the pressure of fresh water is 0.433psi/ft
    //and velocity with Bernoulli's Equation
    pressure_depth = 14.7 + (pressure_static * 0.433);
    pressure_velocity = sqrt((2*abs(pressure_stagnation-pressure_static))/1000);
}





