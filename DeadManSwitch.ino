
/* DeadManSwitch.ino
 * 
 * This file contains the variables and functions needed for the
 * Dead Man Switch (dms) system.
 *
 * The system runs off of 12V and consists of a pusbutton and solenoid in series.
 * There is a voltage divider with resistors 330K/680K-ohm in parallel with the
 * solenoid, but after the pushbutton. When the button is pressed, we will read
 * 12V on the voltage divider, but that will become 0V is the button is released.
 *
 * The solenoid will automatically release the buoy if the pushbutton is un-pressed.
 *
 * This program only monitors whether the solenoid is receiving power.
 *
*/


//pins
int pin_dms = A6; //analog pin 6


//Dead Man Switch variables
bool dms_isbuoydeployed;    //has the buoy been deployed? true if yes, false if no
bool dms_issolenoidengaged; //is the solenoid receiving power? true if yes, false if no 


//setup() function runs once
void setup(){

    dms_setup();

    Serial.begin(9600);
}


//loop() function runs forever
void loop(){
    dms_update();

    Serial.print("isbuoydeployed: ");
    Serial.print(dms_isbuoydeployed);
    Serial.print(" issolenoidengaged: ");
    Serial.println(dms_issolenoidengaged);

}





//this function initializes the dms variables
void dms_setup(){
    dms_isbuoydeployed = false;
    dms_issolenoidengaged = false;
}

//pin_dms should read 4V (819/1024) if the solenoid is engaged,
//and 0V (0/1024) if there is no power to the solenoid
//We use 410 here because it is in the middle, 
//which accounts for variable input power supply
void dms_update(){
    bool previous = dms_issolenoidengaged;
    if (analogRead(pin_dms) > 410) dms_issolenoidengaged = true;
    
    //if the solenoid was previously engaged, and is not engaged anymore,
    //then the buoy has been deployed, and we should update the buoy status
    if (previous && !(dms_issolenoidengaged))
        dms_isbuoydeployed = true;
}








