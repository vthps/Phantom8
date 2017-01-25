
/*
 * RPM_DRIVER.ino
 * This file contains functions that will:
 * 1) Calculate RPM at any time 
 * 2) Setup the pin interrupts to allow said
 *    calculations to occur
 */


// rpm interrupt pins
const int pin_rpm_interrupt_1 = 22;
const int pin_rpm_interrupt_2 = 24;

// rpm calculation variables
int rpm_hit_count;
double rpm_current;
unsigned long rpm_time;
int TIME_THRESHOLD = 60000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  rpm_sensor_setup();
  Serial.print("setup complete\n");
}

void loop() {
  double rpm_temp = rpm_calculate();

Serial.print(analogRead(pin_rpm_interrupt_1));
Serial.print("\t");
Serial.print(analogRead(pin_rpm_interrupt_2));

  if (rpm_temp != rpm_current) {
    rpm_current = rpm_temp;
    Serial.print("\t");
    Serial.print(rpm_current);
  }  
Serial.print("\n");
}
/*
 * Run this code at some point in the
 * setup method to initialize RPM sensor
 * interrupts
 */
void rpm_sensor_setup() {
  attachInterrupt(digitalPinToInterrupt(pin_rpm_interrupt_1), rpm_count_increment, RISING);
  attachInterrupt(digitalPinToInterrupt(pin_rpm_interrupt_2), rpm_count_increment, RISING);
  pinMode(pin_rpm_interrupt_1, INPUT);
  pinMode(pin_rpm_interrupt_2, INPUT);
}

/*
 * Run this code at some point in the loop
 * to calculate rpm since the last loop
 * 
 * for code convenience, please use:
 * 
 * rpm_current = rpm_calculate();
 */
double rpm_calculate() {
  if (millis() - rpm_time >= TIME_THRESHOLD) {
    unsigned long tmp_time = rpm_time;
    rpm_time = millis();
    double rpm = (((double) 60000* rpm_hit_count) / (rpm_time - tmp_time)) / 2; 
    rpm_hit_count = 0;         
    return rpm;
  } else {
    return rpm_current;
  }
}
/*
 * Function called only when rpm_interrupt occurs
 * 
 * DO NOT CALL THIS FROM ANY POINT IN THE CODE!!!
 */
void rpm_count_increment(){
  Serial.print("hit ");
  rpm_hit_count++;
}
