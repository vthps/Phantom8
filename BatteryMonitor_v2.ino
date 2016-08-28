
/*
* BatteryMonitor_v2.ino
*
* This file contains a function that will:
* 1) measure the voltage from the battery
* 2) measure the current from the current sensor
* 3) calculate how many milli-watt-seconds have been used by the battery
* 4) calculate the percentage of energy left in the battery
*/


//battery constants
const double BATTERY_CAPACITY = 277056000; //[mWs] (milli-watt-seconds) == 76.96 watt-hours

//battery variables
double battery_voltage;  //instantaneous voltage read by voltage divider on battery input
double battery_current;  //instantaneous current read by current sensor
double battery_watts;    //instantaneous watts = instantaneous voltage * instantaneous current
double battery_energy;   //[mWs] cumulative energy used so far, integral of instantaneous watts dt
double battery_lifeleft; //percentage of life left (subtracting battery_energy from BATTERY_CAPACITY)
unsigned long battery_prevtime;  //[ms] the time in milliseconds when we last read battery info
unsigned long battery_deltatime; //[ms] time elapsed since we last read battery info

//battery pins
int pin_battery_voltage = 0;  //analog pin
int pin_battery_current = 9; //analog pin

//analog pins
int pin_pressure_a_shield2 = 6; //analog pin
int pin_pressure_a_shield3 = 7; //analog pin
int pin_pressure_b_shield2 = 8; //analog pin
int pin_pressure_b_shield3 = 9; //analog pin


void setup()
{
    battery_prevtime = 0;
    battery_energy = 0;

    //pinMode(pin_battery_voltage, INPUT);
    //pinMode(pin_battery_current, INPUT);
    Serial.begin(9600);
}

void loop()
{
    //calculate the values
    battery_calculate_all();


    //print results
    Serial.print("battery_voltage= ");
    Serial.print(battery_voltage);
    Serial.print(" battery_current= ");
    Serial.print(battery_current);

    Serial.print(" battery_watts= ");
    Serial.print(battery_watts);
    Serial.print(" battery_energy= ");
    Serial.print(battery_energy);
    Serial.print(" battery_lifeleft= ");
    Serial.println(battery_lifeleft);

    //also print a few analog pins (not related to the battery)
    /*
    Serial.print(" pin_pressure_a_shield2= ");
    Serial.print(pin_pressure_a_shield2);
    Serial.print(" pin_pressure_a_shield3= ");
    Serial.print(pin_pressure_a_shield3);
    Serial.print(" pin_pressure_b_shield2= ");
    Serial.print(pin_pressure_b_shield2);
    Serial.print(" pin_pressure_b_shield3= ");
    Serial.println(pin_pressure_b_shield3);
    */
}

//this function updates the variables with prefix battery_
void battery_calculate_all(){
    //how much time has elapsed since the last reading?
    battery_deltatime = millis() - battery_prevtime;
    battery_prevtime = millis();

    //read the values
    battery_voltage = analogRead(pin_battery_voltage);
    battery_current = analogRead(pin_battery_current);
    
    //convert to volts, assume perfect 5V supply
    battery_voltage = (battery_voltage/1024) * 5;
    battery_current = (battery_current/1024) * 5;

    //account for inaccurate readings through analogRead
    battery_voltage -= 0.1;
    
    //calculate the values
    battery_voltage = battery_voltage * 3.543;    //applying voltage division formula
    if (battery_voltage < 0) battery_voltage = 0; //might read negative if the system power hasn't been turned on yet
    battery_current = battery_current*1.1;        //experimentally derived trendline converting sensor voltage to current
    battery_watts = battery_voltage * battery_current;              //instantaneous power
    battery_energy += battery_deltatime * battery_watts;            //add the area of this rectangle to the running sum
    battery_lifeleft = ( 1-(battery_energy/BATTERY_CAPACITY) )*100; //starts at 100% and counts down down
}






