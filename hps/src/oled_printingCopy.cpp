
#include "oled_printingCopy.h"

PrintHPS::PrintHPS()
{
    oled = new Adafruit_SSD1306(4);
    size = 0;
    index = 0;
    data = new HPSVals[size];
	extra_char_in_header = '~';
}

PrintHPS::PrintHPS(int size)
{
    oled = new Adafruit_SSD1306(4);
    this->size = size;
    index = 0;
    data = new HPSVals[size];
	extra_char_in_header = '~';
}

void PrintHPS::setSize(int size)
{
    this->size = size;
    delete data;
    data = new HPSVals[size];
}

const char* PrintHPS::add(HPSVals newEntry)
{
    if (index < size)
    {
        data[index] = newEntry;
        index++;
        return "Entry added.";
    }
    return "Addition failed - No more room in the array.";
}

const char* PrintHPS::add(char* name, char* unit, void* variable, int decimal_places, int screenType)
{
	if (index < size)
	{
		HPSVals newEntry;
		newEntry.name_of_variable = name;
		newEntry.unit = unit;
		newEntry.variable = variable;
		newEntry.decimal_places = decimal_places;
		newEntry.screenType = screenType;
		data[index] = newEntry;
		index++;
		return "Entry Added.";
	}
	return "Addition failed - No more room in the array.";
}
		
void PrintHPS::resetIndex()
{
    index = 0;
}

void PrintHPS::setIndex(int index)
{
    this->index = index;
}

int PrintHPS::getNext()
{
    int output = index;
    output++;
    if (output == size) output = 0; //wrap back to the beginning if we advance too far
    return output;
}

int PrintHPS::getPrev()
{
    int output = index;
    output--;
    if (output < 0) output = size-1; //go to the last element if we go back too far
    return output;
}

int PrintHPS::getScreenType()
{
    return data[index].screenType;
}

void PrintHPS::setHeaderChar(char c){
	extra_char_in_header = c;
}

//does not print the value of the varible, only sets up the screen with the title (in yellow) and prev/next indicators
int PrintHPS::printHeaders()
{
	int retval = 0;
	if (data[index].screenType == 0){
		oled->clearDisplay();
		oled->setTextColor(WHITE, BLACK);
		oled->setCursor(0,0);
		oled->setTextSize(2);
		oled->print("Zero!");
		if (extra_char_in_header != '~'){
			oled->print(" ");
			oled->print(extra_char_in_header);
		}
		oled->println("");
		oled->setCursor(0,25);
		oled->setTextSize(2);
		oled->print("Auto");
		//oled->println("");
		//oled->setCursor(0,50);
		//oled->setTextSize(2);
		//oled->print("Manual");
	}
	else if (data[index].screenType == 1){
		//autonomous screen
		oled->clearDisplay();
		oled->fillTriangle(54, 10, 54, 60, 110, 30, WHITE);
	}
	else if (data[index].screenType == 2){
		oled->clearDisplay();
		oled->setTextColor(WHITE, BLACK);
		oled->setCursor(40, 30);
		oled->setTextSize(2);
		oled->print("START");
	}
	else{
		oled->clearDisplay();
		oled->setTextColor(WHITE, BLACK);
		oled->setCursor(0,0);
		oled->setTextSize(2);
		oled->print(data[index].name_of_variable);
		if (extra_char_in_header != '~'){
			oled->print(" ");
			oled->print(extra_char_in_header);
		}
		oled->println("");
		oled->setCursor(0,56);
		oled->setTextSize(1);
		oled->print("< ");
		oled->print(data[getPrev()].name_of_variable);
		oled->print("   ");
		oled->print(data[getNext()].name_of_variable);
		oled->print(" >");
	}
    retval = oled->display();
    return retval;
}

int PrintHPS::menuSelect(int pos)
{
	oled->clearDisplay();
	oled->setTextColor(WHITE, BLACK);
	int retval = 0;
	switch(pos){

		case 1:
			oled->setCursor(0,0);
			oled->setTextSize(2);
			oled->print("Zero!");
			oled->println("");
			oled->setCursor(0,25);
			oled->setTextSize(2);
			oled->print("Auto");
			oled->println("");
			oled->setCursor(0,50);
			oled->setTextSize(2);
			//oled->print("Manual");
			break;
		case 2:
			oled->setCursor(0,0);
			oled->setTextSize(2);
			oled->print("Zero");
			oled->println("");
			oled->setCursor(0,25);
			oled->setTextSize(2);
			oled->print("Auto!");
			oled->println("");
			oled->setCursor(0,50);
			oled->setTextSize(2);
			//oled->print("Manual");
			break;
		/*case 3:
			oled->setCursor(0,0);
			oled->setTextSize(2);
			oled->print("Zero");
			oled->println("");
			oled->setCursor(0,25);
			oled->setTextSize(2);
			oled->print("Auto");
			oled->println("");
			oled->setCursor(0,50);
			oled->setTextSize(2);
			oled->print("Manual!");
			break;*/
	}
	retval = oled->display();
	return retval;
}

int PrintHPS::autoLeft()
{
	int retval = 0;
	if (data[index].screenType == 1){
		//arrow points left
		oled->clearDisplay();
		oled->fillTriangle(44, 30, 100, 10, 100, 60, WHITE);
	}
	retval = oled->display();
	return retval;
}

int PrintHPS::autoRight()
{
	int retval = 0;
	if (data[index].screenType == 1){
		//arrow points right
		oled->clearDisplay();
		oled->fillTriangle(54, 10, 54, 60, 110, 30, WHITE);
	}
	retval = oled->display();
	return retval;
}

int PrintHPS::autoUp()
{
	int retval = 0;
	if (data[index].screenType == 1){
		//arrow points up
		oled->clearDisplay();
		oled->fillTriangle(44, 60, 84, 60, 64, 5, WHITE);
	}
	retval = oled->display();
	return retval;
}

int PrintHPS::autoDown()
{
	int retval = 0;
	if (data[index].screenType == 1){
		//arrow points down
		oled->clearDisplay();
		oled->fillTriangle(44, 5, 84, 5, 64, 60, WHITE);
	}
	retval = oled->display();
	return retval;
}

int PrintHPS::initialize_oled(byte addr)
{
    int retval = 0;
    //the splash screen is in the buffer by default
    oled->begin(SSD1306_SWITCHCAPVCC, addr);
    retval = oled->display();
    return retval;
}

int PrintHPS::printsomething(){
    int retval = 0;
    oled->clearDisplay();
    uint8_t color = WHITE;
    for (int16_t i=0; i<oled->height()/2-2; i+=2)
    {
        oled->fillRoundRect(i, i, oled->width()-2*i, oled->height()-2*i, oled->height()/4, color);
        if (color == WHITE) color = BLACK;
        else color = WHITE;
        retval = oled->display();
    }
    return retval;
}

int PrintHPS::refresh()
{
	if (data[index].screenType == 3)
	{
		int retval = 0;
		oled->setCursor(0,16);
		oled->setTextSize(3);

		if (data[index].decimal_places == 0) oled->print(*(int*)data[index].variable);
		else oled->print(*(double*)data[index].variable, data[index].decimal_places);

		oled->setTextSize(1);
		oled->println(data[index].unit);
		retval = oled->display();
		return retval;
	}
	return 0;
}

void PrintHPS::writeBatteryWarning()
{
	//write code here for a special screen indicating a low battery
	return;
}






