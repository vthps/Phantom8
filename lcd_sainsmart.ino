/*
 * lcd_sainsmart.ino
 * This file contains the code for controlling P7's LCD screen
 * SainSmart IIC/I2C/TWI Serial 2004 20x4 LCD Module
 * Connections: 5V, GND, SDA, SCL
 */

#include <Wire.h>

#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//local variables
float ypr_display[3]; //radians
float depth;
float rpm;
bool zero;

//runs once
void setup() {
    lcd.begin(4, 20); //setting up the LCD screen
    lcd_setupP7();

    //initializing variables to arbitrary quantities so we can display them
    ypr_display[0] = 0.1;
    ypr_display[1] = 0.2;
    ypr_display[2] = 0.3;
    depth = 10;
    rpm = 50.25;
    zero = false;
}

//runs forever
void loop() {
    //pretend to get new values for our variables
    ypr_display[0] += 0.1;
    ypr_display[1] += 0.2;
    ypr_display[2] += 0.3;
    rpm++;
    depth++;
    if (zero == true) zero = false;
    else if (zero == false) zero = true;

    //display on LCD
    lcd_displaydata_P7();

    //pretend there is some delay between displaying
    delay(1000);
}




//Idea: print the labels once, and after that only print the numbers
//this function displays the welcome message and the labels for data that we are going to print
void lcd_setupP7(){

    lcd.setCursor(6, 0); //initial LCD message
    lcd.print("Welcome");
    lcd.setCursor(1, 1);
    lcd.print("Virginia Tech HPS");
    lcd.setCursor(5, 3);
    lcd.print("Phantom 7");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0); //print data labels to LCD
    lcd.print("Yaw");
    lcd.setCursor(10, 1);
    lcd.print("Auto");
    lcd.setCursor(0,1);
    lcd.print("PCH ");
    lcd.setCursor(10, 0);
    lcd.print("RPM");
    lcd.setCursor(0, 2);
    lcd.print("ROL");
    lcd.setCursor(0,3);
    lcd.print("DEP");
    lcd.setCursor(0,3);
}

//this function updates the numbers on the screen
void lcd_displaydata_P7(){
    lcd.setCursor(4, 0);
    lcd_writeFloatNumbers(ypr_display[0] * 180/M_PI); //yaw
    lcd.setCursor(4, 1);
    lcd_writeFloatNumbers(ypr_display[1] * 180/M_PI); //pitch
    lcd.setCursor(4, 2);
    lcd_writeFloatNumbers(ypr_display[2] * 180/M_PI); //roll
    lcd.setCursor(4, 3);
    lcd_writeFloatNumbers(depth);   
    lcd.setCursor(15, 0);
    lcd_writeFloatNumbers(rpm);
    lcd.setCursor(15,1);
    if (zero) lcd.print(" ON");
    else lcd.print("OFF");
}

//This function makes sure that all numbers only take up a certain number of characters
void lcd_writeFloatNumbers(float val) //
{
    //positive value handling
    if (val > 99999)    { lcd.print("XXXX"); }
    else if (val > 999) { lcd.print(val, 0); }
    else if (val > 99)  { lcd.print(val, 1); }
    else if (val > 9)   { lcd.print(val, 2); }
    else if (val == 0)  { lcd.print("0"); }   

    //negative value handling
    else if ( val < 0){
        if (val < -999)     {lcd.print("XXXX");}
        else if (val < -99) { lcd.print(val, 0); }
        else if (val < -9)  { lcd.print(val, 1); } 
        else { lcd.print(val, 2); } 
    }
    
    //values between 0 and 10
    else lcd.print(val, 3);
}







  
