
//This file contains an array of structures,
//where each structure has a different variable available for printing on the OLED

#ifndef OLED_PRINTINGCOPY_H
#define OLED_PRINTINGCOPY_H

#include "WSWire.h"
#include "oled_Adafruit_GFX.h"
#include "oled_Adafruit_SSD1306.h"

struct HPSVals
{
    const char* name_of_variable; //name of variable to display
    const char* unit;             //units, 2 char max
    void* variable;         //pointer to variable
    int decimal_places;     //0 for integer, otherwise assume the datatype is double
	int screenType;         //0 for menu, 1 for autonomous, 2 for manual, 3 for old data screens
};

class PrintHPS
{
    private:
        Adafruit_SSD1306 *oled;
        HPSVals* data;
        int index;
        int size;
		char extra_char_in_header;
		//int screen;

    public:
        PrintHPS();
        PrintHPS(int size);
        void setSize(int size);      //set the size manually
        const char* add(HPSVals newEntry); //adds an entry to the array
		const char* add(char* name, char* unit, void* variable, int decimal_places, int screenType);
		void setHeaderChar(char c);  //sets the variable: char extra_char_in_header
        void resetIndex();           //resets the index to 0
        void setIndex(int index);    //sets the index to a specific number, starts at 0
        int getNext();               //returns the next index in the array (implements a circular array)
        int getPrev();               //returns the previous index in the array (implements a circular array)
		int getScreenType();         //returns current screen type (menu, auto, manual)
        int printHeaders();         //prints the current struct to the OLED
        int initialize_oled(byte addr = 0x3C);      //sets up the interface and displays the splash screen
        int refresh();              //displays the most recent numbers
		void writeBatteryWarning();  //displays a special message to warn people about a low battery
		
		//void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
		void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
		int menuSelect(int pos);    //highlights options on menu screen based on user select
		int autoLeft();
		int autoRight();
		int autoUp();
		int autoDown();

        int printsomething();
};

#endif
