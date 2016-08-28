/*
 * Filename: dpad.ino
 * This file contains the code to read the dpad.
 */

//pins
int pin_dpad = 0; //arduino analog pin 0

//custom data types
enum Direction {NONE=0, UNDEFINED=1, UP=2, DOWN=3, LEFT=4, RIGHT=5, CENTER=6};

//global variables
Direction dpad_direction;


//runs once
void setup() {
    //initialize variables
    dpad_direction = NONE;
    
    Serial.begin(9600);
}


//runs forever
void loop() {

    //Check if any buttons on the dpad have been pressed
    if (detectTransition(pin_dpad, 5, true)){
        dpad_getDirection(); //update the dpad_direction variable
    }
    else dpad_direction = NONE;
    

}








//call this function to read the analog pin and update the dpad_direction variable
void dpad_getDirection(){
    double value = analogRead(pin_dpad); //read the value on pin_dpad
    value = (value/1024)*4.84;           //convert to voltage
    Serial.print(value);
    Serial.print(" ");
    
    //up if the voltage is 0.75-1.25V
    if (value < 1.25 && value > 0.75){
        dpad_direction = UP;
    }

    //down if the voltage is 4.75-5.25V
    else if (value < 5 && value > 4.75){
        dpad_direction = DOWN;
    }

    //left if the voltage is 3.75-4.25V
    else if (value < 4.25 && value > 3.75){
        dpad_direction = LEFT;
    }

    //right if the voltage is 2.75-3.25V
    else if (value < 3.25 && value > 2.75){
        dpad_direction = RIGHT;
    }

    //center if the voltage is 1.75-2.25V
    else if (value < 2.25 && value > 1.75){
        dpad_direction = CENTER;
    }
    
    else dpad_direction = NONE;
}

//This function prints a variable of type Direction so we can read it easily
String printDirection(Direction d){
    if (d == NONE) return "None  ";
    else if (d == UP) return "Up    ";
    else if (d == DOWN) return "Down  ";
    else if (d == LEFT) return "Left  ";
    else if (d == RIGHT) return "Right ";
    else if (d == CENTER) return "Center";
    else return "Dpad printing error";
}

//For switch debouncing!
//Waits a certain amount of time and returns if an edge was detected after the interval
bool detectTransition(int pin, int sample_time_ms, bool is_rising)
{
    unsigned long prevmillis = millis();
    while (millis() < prevmillis+sample_time_ms);

    if (is_rising && analogRead(pin)>100)
        //return true if you were looking for a rising edge and the pin is high
        return true;
    else if (!is_rising && analogRead(pin)<100)
        //return true if you were looking for a falling edge and the pin is low
        return true;
    else
        //return false if there was no transition
        return false;
}





