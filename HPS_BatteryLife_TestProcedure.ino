
/* HPS_BatteryLife_TestProcedure.ino
 *  
 * This program includes the following hardware:
 *    - Datalogger      (Sparkfun Openlog)
 *    - Accelerometer   (Sparkfun MPU6050)
 *    - 4x Servos       (Hitec 33322S HS-322HD Standard Deluxe Karbonite Gear Servo)
 *    - Current Sensor  (Sparkfun Low Current Sensor Breakout ACS712)
 *    - Linear Actuator (Firgelli L16P)
 *    - Dpad Prototype
 *    
*/


//general includes
#include <Servo.h>
#include <WSWire.h>
#include <math.h>

//includes for accelerometer
#include <helper_3dmath.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <MPU6050.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

//includes for oled
#include <oled_printing.h>


//pins
int pin_servo1 = 7; //digital pin 7
int pin_servo2 = 6; //digital pin 6
int pin_servo3 = 4; //digital pin 4
int pin_servo4 = 3; //digital pin 3
int pin_battery_voltage = A0;   //analog pin 0
int pin_battery_current = A9;   //analog pin 9
int pin_datalogger_reset = A10; //analog pin 10
int pin_dpad = A6;     //analog pin 6
int pin_act_pot = A8;  //analog pin for actuator potentiometer // analog input pin
int pin_act_gnd = 17;  //digital out pin attached to pin 9 on H bridge // digital output pin
int pin_act_pos = A11; //digital out pin attached to pin 1 on H bridge // digital output pin


//constants
const double BATTERY_CAPACITY = 277056000; //[mWs] (milli-watt-seconds) = 76.96 watt-hours
const int ACTUATOR_COUNT_THRESHOLD = 1;
const int ACTUATOR_POT_MIN = 202;
const int ACTUATOR_POT_MAX = 1020;
const int ACTUATOR_LENGTH_MIN = 0;
const int ACTUATOR_LENGTH_MAX = 3;
const int ACTUATOR_TOLERANCE = 2;
const int ACTUATOR_STOP_TOL = 6;

//accelerometer variables
MPU6050 mpu;            //accelerometer, class default I2C address is 0x68
int mpu_error_code;     //added by brooke to see I2C errors
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
Quaternion q;           // [w, x, y, z]       quaternion container
VectorFloat gravity;    // [x, y, z]          gravity vector
float ypr_current[3];   // [yaw, pitch, roll] yaw/pitch/roll container and gravity vector


//servo variables
Servo servo1, servo2, servo3, servo4;
int pos;


//battery variables
double battery_voltage;  //instantaneous voltage read by voltage divider on battery input
double battery_current;  //instantaneous current read by current sensor
double battery_watts;    //instantaneous watts = instantaneous voltage * instantaneous current
double battery_energy;   //[mWs] cumulative energy used so far, integral of instantaneous watts dt
double battery_lifeleft; //percentage of life left (subtracting battery_energy from BATTERY_CAPACITY)
unsigned long battery_prevtime;  //[ms] the time in milliseconds when we last read battery info
unsigned long battery_deltatime; //[ms] time elapsed since we last read battery info


//dpad variables
enum Direction {NONE=0, UNDEFINED=1, UP=2, DOWN=3, LEFT=4, RIGHT=5, CENTER=6};
Direction dpad_direction;


//oled variables
PrintHPS *oledController;
int oled_error_code; //added by brooke to see I2C errors


// actuator variables
boolean actuator_target_hit = true;
int actuator_target = -1;
int actuator_perceived_position;
int actuator_hit_count = 0;
int actuator_count_TEST = 1;

//temporary variables
unsigned long prevtime;
unsigned long acctime;
unsigned long oledtime;
unsigned long resettime = 30000;
int resets;
int acctimeouts = 0;


//Interrupt Service Routines (ISR's)
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}


//setup() function runs once
void setup()
{
    //start our serial ports
    Serial.begin(9600);  //Serial0 is for debug output
    Serial3.begin(9600); //Serial3 is for reading from and writing to the datalogger
    
    //specify the direction for all of the digital pins (except the ones being used for servos)
    pinMode(pin_datalogger_reset, OUTPUT);
    
    //initialize variables
    battery_prevtime = 0;
    battery_energy = 0;
    pos = 0;
    resets = 0;

    //initialize our peripherals
    dpad_direction = NONE;
    setupAccelerometer_sparkfun();
    servo1.attach(pin_servo1);
    servo2.attach(pin_servo2);
    servo3.attach(pin_servo3);
    servo4.attach(pin_servo4);
    datalogger_reset(pin_datalogger_reset);
    actuator_setup();

    //set up oled
    oled_error_code = 0;
    oled_setup();
    oledController->setIndex(0); //set the screen to display current, as defined in oled_setup()

    
    prevtime = millis();
    acctime = millis();
}


//loop() function runs forever
void loop()
{
    //update variables
    battery_calculate_all();           //updates all battery variables
    datalogger_CheckForPrintCommand(); //should we print out the datalogger files over USB right now?
    dpad_loop();                       //updates the dpad direction
    actuator_loop();
    getAccelerometerVals_sparkfun();   //updates the ypr_current array with accelerometer info

    Serial.print(acctimeouts);
    
    //move the servos to eat up power
    /*
    if (pos > 90) pos = 0;
    else pos++;
    servo1.write(pos);
    servo2.write(pos);
    servo3.write(pos);
    servo4.write(pos);
    */

    //move the actuator to eat up power
    /*
    if (actuator_target_hit){
      actuator_move_to(actuator_count_TEST % 2);
      Serial.print("Moving to: ");
      Serial.print(actuator_count_TEST++ % 2);
      Serial.print("\n");
    }
    */

    //update the oled
    oled_error_code = oledController->refresh();
    if (oled_error_code != 0){
        Serial.print("oled_error_code=");
        Serial.print(oled_error_code);
        Serial.println(" during oledController->refresh()");
    }

    //testing that the battery monitor is alive
    Serial.print(printDirection(dpad_direction));
    Serial.print(" battery_voltage: ");
    Serial.print(battery_voltage);
    Serial.print(" battery_current: ");
    Serial.print(battery_current);
    
    //testing that the accelerometer is alive
    Serial.print(" ypr_current ");
    Serial.print(ypr_current[0] * 180/M_PI);
    Serial.print(" ");
    Serial.print(ypr_current[1] * 180/M_PI);
    Serial.print(" ");
    Serial.println(ypr_current[2] * 180/M_PI);

    //print to the datalogger
    Serial3.print("ypr_current,");
    Serial3.print(ypr_current[0] * 180/M_PI);
    Serial3.print(",");
    Serial3.print(ypr_current[1] * 180/M_PI);
    Serial3.print(",");
    Serial3.println(ypr_current[2] * 180/M_PI);

}





void setupAccelerometer_sparkfun()
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        //attachInterrupt(0, dmpDataReady, RISING); //Digital I/O pin 2
        //mpuIntStatus = mpu.getIntStatus();

        //added by Brooke 05/11/2016
        //disable all MPU6050 interrupts by writing to register 56 (0x38)
        mpu.setIntEnabled(0);

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        //Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
} //end setupAccelerometer_sparkfun()


void getAccelerometerVals_sparkfun()
{
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // check for overflow (this should never happen unless our code is too inefficient)
    if (fifoCount == 1024){
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));
        mpu_error_code = mpu.get_error_code();
        if (mpu.get_error_code() != 0){
            Serial.print("mpu_error_code=");
            Serial.print(mpu_error_code);
            Serial.println(" during mpu.resetFIFO()");
        }
    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else{
        // wait for correct available data length, should be a VERY short wait
        acctime = millis();
        while (fifoCount < packetSize){
            fifoCount = mpu.getFIFOCount();
            if ((millis() - acctime) > 5000){
                acctimeouts++;
                mpu.resetFIFO();
                Serial.println("while(fifoCount < packetSize) timed out. FIFO reset.");
                return;
            }
        }

        mpu_error_code = mpu.get_error_code();
        if (mpu.get_error_code() != 0){
            Serial.print("mpu_error_code=");
            Serial.print(mpu_error_code);
            Serial.println(" during mpu.getFIFOCount()");
        }

        //added by Brooke 4/20
        int syncData = fifoCount % packetSize;
        if (syncData > 0){
            Serial.println("Data Sync error");
            mpu.resetFIFO();
            mpu_error_code = mpu.get_error_code();
            if (mpu.get_error_code() != 0){
                Serial.print("mpu_error_code=");
                Serial.print(mpu_error_code);
                Serial.println(" during mpu.resetFIFO()");
            }
        }
        else{
            // read a packet from FIFO
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            mpu_error_code = mpu.get_error_code();
            if (mpu.get_error_code() != 0){
                Serial.print("mpu_error_code=");
                Serial.print(mpu_error_code);
                Serial.println(" during mpu.getFIFOBytes()");
            }

            mpu.resetFIFO();
            mpu_error_code = mpu.get_error_code();
            if (mpu.get_error_code() != 0){
                Serial.print("mpu_error_code=");
                Serial.print(mpu_error_code);
                Serial.println(" during mpu.resetFIFO()");
            }
            
        }
        fifoCount -= packetSize;
        
        q.w = (float) ((fifoBuffer[0] << 8) + fifoBuffer[1]) / 16384.0f;
        q.x = (float) ((fifoBuffer[4] << 8) + fifoBuffer[5]) / 16384.0f;
        q.y = (float) ((fifoBuffer[8] << 8) + fifoBuffer[9]) / 16384.0f;
        q.z = (float) ((fifoBuffer[12] << 8) + fifoBuffer[13]) / 16384.0f;

        gravity.x = 2 * (q.x * q.z - q.w * q.y);
        gravity.y = 2 * (q.w * q.x + q.y * q.z);
        gravity.z = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

        ypr_current[0] = atan2( (2 * q.x * q.y) - (2 * q.w * q.z), (2 * q.w * q.w) + (2 * q.x * q.x) - 1);
        ypr_current[1] = atan(gravity.x / sqrt(gravity.y * gravity.y + gravity.z * gravity.z));
        ypr_current[2] = atan(gravity.y / sqrt(gravity.x * gravity.x + gravity.z * gravity.z));
    }
} //end getAccelerometerVals_sparkfun()

//this function updates the variables with prefix battery_
void battery_calculate_all()
{
    //how much time has elapsed since the last reading?
    battery_deltatime = millis() - battery_prevtime;
    battery_prevtime = millis();

    //read the values
    analogRead(pin_battery_voltage);
    battery_voltage = analogRead(pin_battery_voltage);
    analogRead(pin_battery_current);
    battery_current = analogRead(pin_battery_current);

    //convert to volts, assume perfect 5V supply
    battery_voltage = (battery_voltage/1024) * 5;
    battery_current = (battery_current/1024) * 5;

    //calculate the values
    battery_voltage = battery_voltage * 3.543;    //applying voltage division formula
    if (battery_voltage < 0) battery_voltage = 0; //might read negative if the system power hasn't been turned on yet
    else battery_voltage += 0.15;                 //correct for inaccurate readings through analogRead
    battery_current = battery_current*1.1;        //experimentally derived trendline converting sensor voltage to current
    battery_watts = battery_voltage * battery_current;              //instantaneous power
    battery_energy += battery_deltatime * battery_watts;            //add the area of this rectangle to the running sum
    battery_lifeleft = ( 1-(battery_energy/BATTERY_CAPACITY) )*100; //starts at 100% and counts down down
} //end battery_calculate_all()

//This function prints all of the text files on the SD card to Serial0
//It is assumed that the datalogger is on Serial3
void datalogger_printFilesToSerial()
{
    int maximum_number_of_files = 100;         //arbitrary limit so that we can declare the array
    String filenames[maximum_number_of_files]; //an array of filename strings in the form of "LOGXXXXX.TXT"
    String line;                               //a line of information returned by the "ls" command to the datalogger. parse this to get a filename
    byte byteRead;                             //temporary holding variable for a byte received by the serial port (UART)
    int filenames_index = 0;                   //index to keep track of where we are in the filenames[] array
    int i=0;                                   //local iterator to use in for loops
    
    //send 0x24 three times and wait for OpenLog to respond with '>' to indicate we are in command mode
    //0x24 is the ascii value for the OpenLog's escape character '$'
    Serial.println("Attempting to enter command mode...");
    Serial3.write(0x24);
    Serial3.write(0x24);
    Serial3.write(0x24);
    while(1) {
        if(Serial3.available())
            if(Serial3.read() == '>') break;
    }
    Serial.println("Entered command mode.");

    //get a list of all the text files on the SD card
    //each output line from ls is terminated by \r\n
    //then we parse the line for a substring starting with "LOG" and ending with "."
    Serial3.print("ls *.TXT\r");
    while(1){
        if (Serial3.available()){
            byteRead = Serial3.peek();
            if (byteRead == '>'){ 
                //then the command is finished
                byteRead = Serial3.read();
                break;
            }
            else{
                line = Serial3.readStringUntil('\n');
                if (line.startsWith("LOG") && filenames_index < maximum_number_of_files){
                    filenames[filenames_index] = line.substring(0, line.indexOf('.'))+".TXT";
                    filenames_index++;
                }
            }
        }//end if (Serial3.available())
    }//end while

    //print all of the files found above to Serial0
    for (i=0; i<filenames_index; i++){
        Serial3.print("read ");
        Serial3.print(filenames[i]);
        Serial3.print('\r');

        Serial.print("========== ");
        Serial.print(filenames[i]);
        //Serial.print(" ==========");
        while(1){
            if (Serial3.available()){
                byteRead = Serial3.read();
                if (byteRead == '>') break;
                Serial.write(byteRead);
            }
        }
    }

    Serial.print("========== Finished");

    //reset the datalogger to get out of command mode and back into normal data logging mode
    datalogger_reset(pin_datalogger_reset);
}

//This function resets the datalogger
void datalogger_reset(int pin)
{
    digitalWrite(pin, LOW);
    delay(100);
    digitalWrite(pin, HIGH);
}

//this function will check for data received on Serial0
//if the character 'l' (lowercase L) is received, then we will
//call datalogger_printFilesToSerial() and print the datalogger files
//back onto Serial0, where another program is hopefully expecting the output
void datalogger_CheckForPrintCommand()
{
    byte byteRead;
    if (Serial.available()){
        if (Serial.read() == 'l'){
            Serial.println("Attempting to print datalogger files to Serial0...");
            datalogger_printFilesToSerial();
            Serial.println("Finished printing.");
        }
    }
}

void dpad_loop(){
    //Check if any buttons on the dpad have been pressed
    if (detectTransition(pin_dpad, 5, true)){
        dpad_getDirection(); //update the dpad_direction variable
    }
    else dpad_direction = NONE;
}

//call this function to read the analog pin and update the dpad_direction variable
void dpad_getDirection(){
    double value = analogRead(pin_dpad); //read the value on pin_dpad
    value = (value/1024)*4.84;           //convert to voltage

    //up if the voltage is 0.75-1.25V
    if (value < 1.25 && value > 0.75) dpad_direction = UP;

    //down if the voltage is 4.75-5.25V
    else if (value < 5 && value > 4.75) dpad_direction = DOWN;
    
    //left if the voltage is 3.75-4.25V
    else if (value < 4.25 && value > 3.75) dpad_direction = LEFT;

    //right if the voltage is 2.75-3.25V
    else if (value < 3.25 && value > 2.75) dpad_direction = RIGHT;

    //center if the voltage is 1.75-2.25V
    else if (value < 2.25 && value > 1.75) dpad_direction = CENTER;
    
    else dpad_direction = NONE;
}

//This function prints a variable of type Direction so we can read it easily
String printDirection(Direction d){
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
//Assumes the pin was low before calling this function
bool detectTransition(int pin, int sample_time_ms, bool is_rising)
{
    unsigned long prevmillis = millis();
    while (millis() < prevmillis+sample_time_ms);

    if (is_rising && analogRead(pin)>100)
        //return true if you were looking for a rising edge and the pin is high
        return true;
    else if (!is_rising && analogRead(pin)<100)
        //return true if you were looking for a falling edge and the pin is low
        return true;
    else
        //return false if there was no transition
        return false;
}

//This function initizes the oled
void oled_setup()
{
    oledController = new PrintHPS();

    //how many screens are there in total?
    oledController->setSize(3);

    //program the oled with the screens that you want to display
    //"NameOfVariable", "units", &AddressOfVariable, #ofDecimalPlaces
    Serial.println(oledController->add("Yaw", "rad", &ypr_current[0], 3)); //index 0
    Serial.println(oledController->add("Current", "A", &battery_current, 3)); //index 1
    Serial.println(oledController->add("Resets",  "", &resets, 0)); //index 2
    
    oledController->initialize_oled(0x3C);
    delay(2000);
    oledController->resetIndex();
    oledController->printHeaders();
}

/**
 * DO NOT CALL THIS FUNCTION OUTSIDE OF OTHER ACTUATOR FUNCTIONS
 * Sets actuator to retract, extend,
 * or stay put, based on the difference
 * between the target value and the 
 * recorded value
 */
void actuator_set_pins(int difference) {
    //system processing to determine whether to stop the actuator or not
    if (abs(difference) <= ACTUATOR_STOP_TOL) {
        actuator_hit_count++;
        if(actuator_hit_count > ACTUATOR_COUNT_THRESHOLD) {
          actuator_target_hit = true;
          actuator_perceived_position = actuator_target;
        }
    }
    // stop actuator
    if (abs(difference) <= ACTUATOR_TOLERANCE && actuator_target_hit) {
        digitalWrite(pin_act_gnd, LOW);
        digitalWrite(pin_act_pos, LOW);
    }
    //retract actuator
    else if (difference > 0 && !actuator_target_hit) {
      digitalWrite(pin_act_gnd, LOW);
      digitalWrite(pin_act_pos, HIGH);
    }
    //extend actuator
    else if (difference < 0 && !actuator_target_hit) {
      digitalWrite(pin_act_gnd, HIGH);
      digitalWrite(pin_act_pos, LOW);
    }
}

/*
 * Function to be called in setup() of the main 
 * program to setup actuator pins
 */
void actuator_setup() {
    pinMode(pin_act_gnd, OUTPUT);
    pinMode(pin_act_pos, OUTPUT);
}

/*
 * Function that NEEDS to be called at some point in 
 * loop() of the main program, to update the actuator
 * to ensure it does not go too far in either direction
 */
void actuator_loop() {
  int adc_val = analogRead(pin_act_pot);
  
  if (actuator_target < 0) {
    actuator_target = adc_val;
    actuator_perceived_position = adc_val;
  }
  if (actuator_target - actuator_perceived_position != 0) {
    actuator_set_pins(actuator_target - adc_val);
  }
}

/*
 * Function called to move the actuator in an increment
 * e.g. +0.1 to extend a tenth of an inch, -0.1 to retract
 */
void actuator_move_increment(double actuator_distance_change) {
  actuator_move_to(actuator_target + actuator_distance_change);
}

/*
 * Function called to move the actuator to a specified length
 * e.g. 0.5 to move to half an inch, 1.5 to move the an inch and a half
 */
void actuator_move_to(double actuator_length){
  actuator_target = actuator_target_value(actuator_length);
}

/*
 * Function called to stop the actuator in case of emergency
 */
void actuator_stop() {
  int adc_val = analogRead(pin_act_pot);
  actuator_target = adc_val;
  actuator_set_pins(actuator_target - adc_val);
}

/**
 * DO NOT CALL THIS FUNCTION OUTSIDE OF OTHER ACTUATOR FUNCTIONS
 * Returns target value on the potentiometera
 * for a given length (ONLY WORKS FOR WHICHEVER
 * ACTUATOR THE CODE IS CURRENTLY CALIBRATED FOR)
 * CURRENTLY THE WATERPROOFED 63:1 ACTUATOR MARKED (A)
 */
int actuator_target_value(float length) {
    actuator_target_hit = false;
    actuator_hit_count = 0;
    
    if (length < ACTUATOR_LENGTH_MIN) {
      return ACTUATOR_POT_MIN;
    }
    else if (length > ACTUATOR_LENGTH_MAX) {
      return ACTUATOR_POT_MAX;
    }
    else {            
      double val =  24.624*pow(length, 2) + 37.981*length + 204; //calculated model for 
      if (val < ACTUATOR_POT_MIN) {                                     //63:1 actuator A
        return ACTUATOR_POT_MIN;
      }
      else if (val > ACTUATOR_POT_MAX) {
        return ACTUATOR_POT_MAX;
      }
      else {
        return val;
      }
    }
}

