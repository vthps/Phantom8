
/* oled_PrintingForHPS.ino
 *
 * This file contains code for printing to the oled display using a special
 * HPS library. This library makes it easy to implement a scrolling interface
 * where we can have multiple OLED screens with one variable per screen.
 * This scrolling interface was written custom for HPS.
 */

#include <oled_printing.h>
#include <Wire.h>

//oled variables
PrintHPS *oledController;

//general variables (not for the oled)
int voltage;
double current;
double example;
char headerChar;



//runs once
void setup()
{
    voltage = 0;
    current = 0.990;
    example = 0.100;
    headerChar = 'a';

    oled_setup();
    oledController->setIndex(1); //set the screen to display current, as defined in oled_setup()

    Serial.begin(9600);

}


void loop(){
    for (int i=0; i<2; i++)
    {
        voltage++;
        current += 0.1;
        example += 0.2;

        //refresh the oled with the new values for its variables
        //NOTE: this doesn't update the yellow header or the arrows on the bottom
        //we can call this function frequently and not slow things down
        oledController->refresh();

        delay(1000);
    }

    //this sequence is meant to be called less frequently because it takes more time to execute
    //scroll to the next screen (the one to the right)
    //you can also call oledController->getPrev() to scroll left
    oledController->setIndex(oledController->getNext());
    //printHeaders() will update the yellow header and the arrows on the bottom
    headerChar++;
    oledController->setHeaderChar(headerChar);
    oledController->printHeaders();
    oledController->refresh();

    delay(1000);
}



//This function initizes the oled
void oled_setup()
{
    oledController = new PrintHPS();

    //how many screens are there in total?
    oledController->setSize(3);

    //program the oled with the screens that you want to display
    //"NameOfVariable", "units", &AddressOfVariable, #ofDecimalPlaces
    Serial.println(oledController->add("Voltage", "mV", &voltage, 0)); //index 0
    Serial.println(oledController->add("Current", "mA", &current, 3)); //index 1
    Serial.println(oledController->add("Example",  "?", &example, 2)); //index 2
    
    oledController->initialize_oled(0x3C);
    delay(1000);
    oledController->resetIndex();
    oledController->printHeaders();
}

























