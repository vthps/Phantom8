#include <oled_printingCopy.h>
#include <oled_Adafruit_GFX.h>

//oled variables
PrintHPS *oledController;

//general variables (not for the oled)
int voltage;
double current;
double example;
char headerChar;

/**DPAD VARIABLES**/
//pins
int pin_dpad = 0; //arduino analog pin 0

//custom data types
enum Direction {NONE = 0, UNDEFINED = 1, UP = 2, DOWN = 3, LEFT = 4, RIGHT = 5, CENTER = 6};

//global variables
Direction dpad_direction;
int selection = 1;

void setup() {
  // put your setup code here, to run once:
  dpad_direction = NONE;

  Serial.begin(9600);
  Serial.print("NO");
  oled_setup();
  oledController->setIndex(1); //set the screen to display current, as defined in oled_setup()

}

void loop() {

  //Check if any buttons on the dpad have been pressed
  if (detectTransition(pin_dpad, 5, true)) {
    dpad_getDirection(); //update the dpad_direction variable
    //Serial.println(printDirection(dpad_direction));
  }
  else dpad_direction = NONE;

  /***************move switch case into its own private function?*********/

  switch (dpad_direction) {
    case CENTER:
      Serial.println("Center");         //put switch inside if statement above?
      //Serial.println("screen type");
      //Serial.println(oledController->getScreenType());
      pushCenter();
      if (oledController->getScreenType() == 0)
      {
        selection = 1;
      }
      break;
    case UP:
      //highlights whatever option is up, update select; does nothing if nothing is up
      if (selection > 1)
      {
        selection = selection - 1;
        oledController->menuSelect(selection);
      }
      Serial.println("Up");
      Serial.println(selection);
      break;
    case DOWN:
      //highlights whatever is down, update select; does nothing if nothing is down
      if (selection < 2)
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

  delay(150);
}

//This function initizes the oled
void oled_setup()
{
  oledController = new PrintHPS();

  //how many screens are there in total?
  oledController->setSize(2);

  //program the oled with the screens that you want to display
  //"NameOfVariable", "units", &AddressOfVariable, #ofDecimalPlaces, screenType
  //Serial.println(oledController->add("Start", "", &voltage, 0, 2)); //index 0
  Serial.println(oledController->add("Menu", "", "Zero", 0, 0)); //index 1
  Serial.println(oledController->add("Autonomous",  "", &example, 2, 1)); //index 2


  oledController->initialize_oled(0x3C);
  delay(1000);
  oledController->resetIndex();
  oledController->printHeaders();
}

/********************************DPAD FUNCTIONS***********************************************/

//call this function to read the analog pin and update the dpad_direction variable
void dpad_getDirection() {
  double value = analogRead(pin_dpad); //read the value on pin_dpad
  value = (value / 1024) * 4.84;       //convert to voltage
  Serial.print(value);
  Serial.print(" ");

  //up if the voltage is 0.75-1.25V
  if (value < 1.25 && value > 0.75) {
    dpad_direction = UP;
  }

  //down if the voltage is 4.75-5.25V
  else if (value < 5 && value > 4.75) {
    dpad_direction = DOWN;
  }

  //left if the voltage is 3.75-4.25V
  else if (value < 4.25 && value > 3.75) {
    dpad_direction = LEFT;
  }

  //right if the voltage is 2.75-3.25V
  else if (value < 3.25 && value > 2.75) {
    dpad_direction = RIGHT;
  }

  //center if the voltage is 1.75-2.25V
  else if (value < 2.25 && value > 1.75) {
    dpad_direction = CENTER;
  }

  else {
    dpad_direction = NONE;
  }
}

//This function prints a variable of type Direction so we can read it easily
String printDirection(Direction d) {
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
  while (millis() < prevmillis + sample_time_ms);
  if (is_rising && analogRead(pin) > 100)
    //return true if you were looking for a rising edge and the pin is high
    return true;
  else if (!is_rising && analogRead(pin) < 100)
    //return true if you were looking for a falling edge and the pin is low
    return true;
  else
    //return false if there was no transition
    return false;
}

void pushCenter() {
  //if selection = 1, zero
  //if selection = 2, switch to autonomous screen -----> include speed readout?
  //if selection = 3, switch to manual screen              //add pitch reset

  if (selection != 1) {
    /********SHIFT SCREEN CODE**********/
    //Serial.println("hello");
    //this sequence is meant to be called less frequently because it takes more time to execute
    //scroll to the next screen (the one to the right)
    //you can also call oledController->getPrev() to scroll left
    oledController->setIndex(oledController->getNext());
    //printHeaders() will update the yellow header and the arrows on the bottom
    headerChar++;
    //oledController->setHeaderChar(headerChar);
    oledController->printHeaders();
    oledController->refresh();

    //selection = 1; //changed screens, so reset selection to 1              UPDATE 2/22/17: make sure the selection is only used on menu screen
  }
}

