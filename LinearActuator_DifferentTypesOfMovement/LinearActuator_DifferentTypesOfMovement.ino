

int pin_prev = 0;
int pin_next = 1;
int pin_hbridge_1 = 2;
int pin_hbridge_2 = 3;

unsigned long period;   //[ms]
unsigned long prevtime; //[ms]
int dir = 0;

int on_time;
int delay_time;

int prev;
int next;

void setup(){
    period = 500;
    pinMode(pin_hbridge_1, OUTPUT);
    pinMode(pin_hbridge_2, OUTPUT);
    Serial.begin(9600);
}


void loop(){
    prev = analogRead(pin_prev);
    next = analogRead(pin_next);
    
    Serial.print("pin_prev: ");
    Serial.print(prev);
    Serial.print(" pin_next: ");
    Serial.print(next);
    Serial.print(" period: ");
    Serial.println(period);
    
    //move_by_button();
    //move_by_freq();
    move_by_pwm();
}

//goes at a fixed PWM speed, in or out depending on which button is pressed
void move_by_pwm(){
    on_time=7;
    delay_time = 14;
    period = on_time + delay_time;
    
    digitalWrite(pin_hbridge_1, HIGH);
    digitalWrite(pin_hbridge_2, HIGH);
    delay(delay_time);
    
    if (prev > 800){
        digitalWrite(pin_hbridge_1, HIGH);
        digitalWrite(pin_hbridge_2, LOW);
        delay(on_time);
    }
    else if (next > 800){
        digitalWrite(pin_hbridge_1, LOW);
        digitalWrite(pin_hbridge_2, HIGH);
        delay(on_time);
    }
}


//goes in and out at a certain frequency
void move_by_freq(){
  if (prev>800) period = period/2;      //slow down
  else if (next>800) period = period*2; //speed up

  //change direction
  if (millis()-prevtime > period){
    
      //stop before changing direction
      digitalWrite(pin_hbridge_1, HIGH);
      digitalWrite(pin_hbridge_2, HIGH);
      delay(period/10);
      
      if (dir==0){
          digitalWrite(pin_hbridge_1, HIGH);
          digitalWrite(pin_hbridge_2, LOW);
          prevtime = millis();
          dir = 1;
      }
      else if (dir==1){
          digitalWrite(pin_hbridge_1, LOW);
          digitalWrite(pin_hbridge_2, HIGH);
          prevtime = millis();
          dir = 0;
      }
  }
}

//goes at a constant speed in or out depending on which button is pressed
void move_by_button(){
    if (prev>800){
        digitalWrite(pin_hbridge_1, HIGH);
        digitalWrite(pin_hbridge_2, LOW);
    }
    else if (next>800){
        digitalWrite(pin_hbridge_1, LOW);
        digitalWrite(pin_hbridge_2, HIGH);
    }
    else{
        digitalWrite(pin_hbridge_1, HIGH);
        digitalWrite(pin_hbridge_2, HIGH);
    }
}




