#include <oled_printingCopy.h>

//oled variables
PrintHPS *oledController;

//general variables (not for the oled)
int voltage;
double current;
double example;
char headerChar;


/* void loop() {

  //Check if any buttons on the dpad have been pressed
    if (detectTransition(pin_dpad, 5, true)){
        dpad_getDirection(); //update the dpad_direction variable
        //Serial.println(printDirection(dpad_direction));
    }
    else dpad_direction = NONE;

 
    
    switch (dpad_direction) {
      case CENTER:
          Serial.println("Center");         //put switch inside if statement above?
          pushCenter();
        break;
      case UP:
        //highlights whatever option is up, update select; does nothing if nothing is up
        if(selection > 1)
        {
          selection = selection - 1;
          oledController->menuSelect(selection);
        }
        Serial.println("Up");
        Serial.println(selection);
        break;
      case DOWN:
        //highlights whatever is down, update select; does nothing if nothing is down
        if(selection < 3)
        {
          selection = selection + 1;
          oledController->menuSelect(selection);
        }
        Serial.println("Down");
        Serial.println(selection);
        break;
      case LEFT:
        //in the menu screen, does nothing
        Serial.println("L");
        break;
      case RIGHT:
        //in the menu screen, does nothing
        Serial.println("R");
        break;
      default: 
        // if nothing else matches aka dpad_direction = NONE
        //does nothing
        //Serial.println("None");
      break;
    }
    /* //this sequence is meant to be called less frequently because it takes more time to execute
    //scroll to the next screen (the one to the right)
    //you can also call oledController->getPrev() to scroll left
    oledController->setIndex(oledController->getNext());
    //printHeaders() will update the yellow header and the arrows on the bottom
    headerChar++;
    oledController->setHeaderChar(headerChar);
    oledController->printHeaders();
    oledController->refresh();

    delay(150);
} 

*/

//This function initizes the oled
void oled_setup() {
    oledController = new PrintHPS();

    //how many screens are there in total?
    oledController->setSize(3);

    //program the oled with the screens that you want to display
    //"NameOfVariable", "units", &AddressOfVariable, #ofDecimalPlaces
    Serial.println(oledController->add("Cynthia", "mV", &voltage, 0, 0)); //index 0
    Serial.println(oledController->add("Menu", "", "Zero", 0, 3)); //index 1
    Serial.println(oledController->add("Zhuang",  "?", &example, 2, 3)); //index 2


    oledController->initialize_oled(0x3C);
    delay(1000);
    oledController->resetIndex();
    oledController->printHeaders();
}



