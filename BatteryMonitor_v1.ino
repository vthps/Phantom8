//WattMeterTest2
//=============
//Determines the number of watts used by the battery by measuring voltage and current periodically
//  then multiplying by the time step between readings (approximating the integral for power/time)
//Number of watts used can then be used to determine power left in the battery
//
//Now reads in voltage and current using the parallel resistor method
// ==============================================
const boolean DEBUG = true; //Debug flag for use in possible testing

const int PIN_PWRIND_ONE = 0; //Value for one of the pins for the power indicator
const int PIN_PWRIND_TWO = 1; // Analog pins A0-A7 are pins 54-61 on the Max32
const int LCD_BAUD_RATE = 1200; //Rates for our LCD are 1200, 2400, 4800, and 9600, based on the datasheet

const int WATT_CAPACITY = 9999; //Total watts available to battery, to be found during testing and changed
const float REFERENCE_VOLTS = 5; //Value for the max input voltage to be read (3.3 for the Max32)
const float ANALOG_STEPS = 1023; //Analog pin resolution

const float RESISTANCE_ONE = 3900; //Resistance of R1 from voltage divider
const float RESISTANCE_TWO = 1000; //Resistance of R2
const float RES_FACTOR = (RESISTANCE_ONE + RESISTANCE_TWO)/RESISTANCE_ONE;
const float SHUNT_RESISTANCE = 110;

float wattsUsed = 0; //Watts used so far (starts at 0)
int previousTime = 0; //Stores the time from the previous measurement to measure the timestep
// ===============================================

void setup()
{
  //Pins default to input, but doing it anyway
  pinMode(PIN_PWRIND_ONE, INPUT);
  pinMode(PIN_PWRIND_TWO, INPUT);
  Serial.begin(9600);
}

void loop()
{
  //Prints current battery status to the LCD
  delay(1000);
  checkBattery();
  
}

//Checks the battery status and prints it out
void checkBattery()
{
  float powerUsed = getPowerUsage();
  
  //Measures the time passed from the previous measurement, 
  //  sets the previous time to now
  int currentTime = millis();
  int timeStep = (millis() - previousTime);
  previousTime = currentTime;
  
  //Multiplies powerUsed by time interval since last check to find area under curve,
    // then adds to current total
  wattsUsed += ((powerUsed * float((timeStep/1000))/3600)*1000); //Time converted into seconds from milliseconds and then hours from seconds
  
  //Determines percentage of watts left for use assuming battery contains WATT_CAPACITY number of watts
  float percentage = (((WATT_CAPACITY - wattsUsed)/(WATT_CAPACITY)) * 100);
  
  //Outputs the percentage to the display
  if (!DEBUG)
    toDisplay(percentage);
  else
  {
    Serial.print("Inst. Watts: ");
    Serial.print(powerUsed,6);
    //Serial.print(((powerUsed * float((timeStep/1000))/3600))*1000);
    Serial.print(" ");
    toDisplay(wattsUsed); 
  }
}

//Returns the power usage at the current instant
float getPowerUsage()
{
  /*
  float voltageDivider1 = (analogRead(pinBefore)/analogSteps)*((voltageDividerR1+voltageDividerR2)/voltageDividerR2)*refVolts;
float voltageDivider2 = (analogRead(pinAfter)/analogSteps)*((voltageDividerR1+voltageDividerR2)/voltageDividerR2)*refVolts;

 //Voltage coming out of the battery
 voltsFromBattery = voltageDivider1;
 
 //Current using V=IR, I=V/R
 currentFromBattery = (voltageDivider1-voltageDivider2)/resistance;
 
 //Power using P=IV;
 powerFromBattery = currentFromBattery * voltsFromBattery;
  */
  
  //Read values from the analog pins
  float voltageDivider1 = (analogRead(PIN_PWRIND_ONE)/ANALOG_STEPS)*((RESISTANCE_ONE+RESISTANCE_TWO)/RESISTANCE_TWO)*REFERENCE_VOLTS;
  float voltageDivider2 = (analogRead(PIN_PWRIND_TWO)/ANALOG_STEPS)*((RESISTANCE_ONE+RESISTANCE_TWO)/RESISTANCE_TWO)*REFERENCE_VOLTS; //Voltage after the parallel resistors
 
  //The voltage coming out of the battery is voltsBefore
  float voltsFromBattery = voltageDivider1;
  
  //Find the current using V=IR, I=V/R
  float currentFromBattery = (voltageDivider1-voltageDivider2)/SHUNT_RESISTANCE;
 
  //Find the power using P=IV
  float powerFromBattery = currentFromBattery * voltsFromBattery;
  
  Serial.print(analogRead(PIN_PWRIND_ONE));
  Serial.print(" ");
  Serial.print(analogRead(PIN_PWRIND_TWO));
  Serial.print("  | ");  
  Serial.print("Volts Before: ");
  Serial.print(voltageDivider1,4);
  Serial.print(" Volts After: ");
  Serial.print(voltageDivider2,4);
  Serial.print(" Current: ");
  Serial.print(currentFromBattery,6);
  
  return powerFromBattery;
}








void toDisplay(float value)
{
  if(!DEBUG)
  {
    Serial.begin(LCD_BAUD_RATE);
    Serial.print(byte(017)); //Control code for moving cursor position
    
    //Moving cursor position to bottom right corner
      //      0  -  19
      // -------------------  0
      // -------------------  |
      // ---------------X---  3
    Serial.print(byte(016)); //Column 16 (0-19)
    Serial.print(byte(003)); //Row 3 (0-3)
    
    //Printing out the value in the form
    // ---------------XXX%
    Serial.print((int)value);
    Serial.print("%");
  }
  else
  {
    //Assuming we'll just hook it up to the computer when testing for battery life
    Serial.print(" Watts Used: ");
    Serial.println(value,4);
  }
}
