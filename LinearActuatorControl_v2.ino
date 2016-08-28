
#include "TimerOne.h"

//digital output pins
int pin_relay0_en = 10;
int pin_relay1_en = 11;
int pin_relay2_en = 12;
int pin_bridge0 = 4;
int pin_bridge1 = 5;

//analog input pins
int pin_act0_pot = 3;
int pin_act1_pot = 4;
int pin_act2_pot = 5;

//variables
int time = 0;
int count = 0;
int act0_pot_val, act1_pot_val, act2_pot_val;
int DELAY = 2000; //[ms]
int DELAY_OUT = 2050; //[ms]
int DELAY_IN = 2000; //[ms]

//helper functions
void act0_en();
void act1_en();
void act2_en();
void act_move_stop();
void act_move_ccw();
void act_move_cw();

//setup - runs once
void setup(){
    //Timer1.initialize(500000); //half second period
    Timer1.initialize(1000); //1 millisecond period
    Timer1.attachInterrupt(time_inc);
    pinMode(pin_relay0_en, OUTPUT);
    pinMode(pin_relay1_en, OUTPUT);
    pinMode(pin_relay2_en, OUTPUT);
    pinMode(pin_bridge0, OUTPUT);
    pinMode(pin_bridge1, OUTPUT);
    Serial.begin(9600);
}

//loop - runs forever
void loop(){
    Serial.println("all stop");
    act_move_stop();
    wait_and_print_pot(DELAY);
        if (count == 0){
			act_move_cw();
			act0_en();
			wait_and_print_pot(DELAY_OUT);
			act1_en();
			wait_and_print_pot(DELAY_OUT);
			act2_en();
			wait_and_print_pot(DELAY_OUT);
                        count = 1;
		}
		else if (count == 1){
			act_move_ccw();
			act0_en();
			wait_and_print_pot(DELAY_IN);
			act1_en();
			wait_and_print_pot(DELAY_IN);
			act2_en();
			wait_and_print_pot(DELAY_IN);
                        count = 0;
		}

}


void time_inc(){
    time++;
}

void wait_and_print_pot(int duration_ms){
    int stoptime = time + duration_ms;
    while (time < stoptime){
        act0_pot_val = analogRead(pin_act0_pot);
        act1_pot_val = analogRead(pin_act1_pot);
        act2_pot_val = analogRead(pin_act2_pot);
        Serial.print("time ");
        Serial.print(time);
        Serial.print(" | ");
        Serial.print("act0 pot = ");
        Serial.print(act0_pot_val);
        Serial.print(" act1 pot = ");
        Serial.print(act1_pot_val);
        Serial.print(" act2 pot = ");
        Serial.println(act2_pot_val);
    }
    time = 0;
}

void act0_en(){
    digitalWrite(pin_relay0_en, HIGH);
    digitalWrite(pin_relay1_en, LOW);
    digitalWrite(pin_relay2_en, LOW);
}

void act1_en(){
    digitalWrite(pin_relay0_en, LOW);
    digitalWrite(pin_relay1_en, HIGH);
    digitalWrite(pin_relay2_en, LOW);
}

void act2_en(){
    digitalWrite(pin_relay0_en, LOW);
    digitalWrite(pin_relay1_en, LOW);
    digitalWrite(pin_relay2_en, HIGH);
}

void act_move_ccw(){
    digitalWrite(pin_bridge0, HIGH);
    digitalWrite(pin_bridge1, LOW);
}

void act_move_cw(){
    digitalWrite(pin_bridge0, LOW);
    digitalWrite(pin_bridge1, HIGH);
}

void act_move_stop(){
    digitalWrite(pin_bridge0, LOW);
    digitalWrite(pin_bridge1, LOW);
}


