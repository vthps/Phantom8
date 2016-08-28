/*
 * RPM_DRIVER.ino
 * This file contains functions that will:
 * 1) Calculate RPM at any time 
 * 2) Setup the pin interrupts to allow said
 *    calculations to occur
 */


// rpm interrupt pins
const int pin_rpm_interrupt_1 = 18;
const int pin_rpm_interrupt_2 = 19;

// rpm calculation variables
int rpm_hit_count_old = 0;
int rpm_hit_count_current = 0;

double rpm_current;

unsigned long rpm_time_1 = 0;
unsigned long rpm_time_2 = 0;



/*
 * Run this code at some point in the
 * setup method to initialize RPM sensor
 * interrupts
 */
void rpm_sensor_setup() {
  pinMode(pin_rpm_interrupt_1, OUTPUT);
  pinMode(pin_rpm_interrupt_2, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(pin_rpm_interrupt_1, rpm_count_increment, RISING);
  attachInterrupt(digitalPinToInterrupt(pin_rpm_interrupt_2, rpm_count_increment, RISING);

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
    double rpm = ((double) (rpm_hit_count_current - rpm_hit_count_old) /
                  (((double)(rpm_time_2 - rpm_time_1))/ 60000.)) / 2.; // divide by 60k to get min from ms
    rpm_hit_count = 0;                                                 // divide by 2 due to 2 sensors
    rpm_time_1 = rpm_time_2;
    rpm_time_2 = millis();

    rpm_hit_count_old = rpm_hit_count_current;

    return rpm;
}

/*
 * Function called only when rpm_interrupt occurs
 * 
 * DO NOT CALL THIS FROM ANY POINT IN THE CODE!!!
 */
void rpm_count_increment(){
  rpm_hit_count_current++;
}

