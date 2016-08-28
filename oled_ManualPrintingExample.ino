
#include <Wire.h>
#include <oled_Adafruit_GFX.h>
#include <oled_Adafruit_SSD1306.h>

Adafruit_SSD1306 oled(4);
double value;

void setup(){
    oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    
    //Adafruit splash screen
    oled.display();
    delay(1000);
    oled.clearDisplay();

    //Write title screen
    oled.setTextColor(WHITE, BLACK);
    oled.setCursor(0,0);
    oled.setTextSize(2);
    oled.println("Voltage");
    oled.setCursor(0,56);
    oled.setTextSize(1);
    oled.print("< prev         next >");
    oled.display();
    
    delay(1000);

    value = 9.995;
}

void loop(){
    oled.setCursor(0,16);
    oled.setTextSize(3);
    oled.print(value,3);
    oled.setTextSize(1);
    oled.println("mV");
    oled.display();
    delay(1000);

    value += 0.001;
}
