#include <Servo.h>
#include "Arduino.h"
#include "common.h"
#include "controls.h"


#include "src/I2Cdev.h" 
#include "src/MPU6050_6Axis_MotionApps20.h" //This header also includes libraries/MPU6050.h
#include "src/helper_3dmath.h"


// Degree position the servos will be initialized to during setup
int LRPosition = 90;
int UDPosition = 90;

//Instance of Direction enum
Direction dpad_direction; 

//Variable that will hold the change to be executed by the servos
int LRchange = 0;
int UDchange = 0;

//array to hold the target values for the sub's trajectory
float BaseLine[3]; // pitch, yaw, roll

//boolean for use of autonomous controls vs manual controls
bool reset = false;

//control variables for switching between autonomous and manual control
int DpadControl = 21;
int DpadControlZero = 21;

//Number of points used to calculate the derrivative part of the PID controller
const int NumSlopePoints = 10;

//PID data variables for the Yaw PID controller
int YawSlopePoints[NumSlopePoints];

int YawP = 0;
int YawI = 0;
int YawD = 0;

float YawPCoefficient = 1;
float YawICoefficient = 0.2;
float YawDCoefficient = 0.2;

//PID data variables for the Pitch PID controller
int PitchSlopePoints[NumSlopePoints];

int PitchP = 0;
int PitchI = 0;
int PitchD = 0;

float PitchPCoefficient = 1;
float PitchICoefficient = 0.2;
float PitchDCoefficient = 0.2;

//Variables for manual course correction 
float CourseCorrectionIncrement = (3.14/180); //amount the baseline changes when drift corretion is used
int Override = 2; //amount the servo changes when immediate override is used

//Declaring servos
Servo LR;
Servo LR2;
Servo UD1;
Servo UD2;

//declaring peripherals
MPU6050 mpu;  // class default I2C address is 0x68

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
// yaw: (about Z axis)
// pitch: (nose up/down, about Y axis)
// roll: (tilt left/right, about X axis)
Quaternion q;             // [w, x, y, z]         quaternion container
VectorFloat gravity;      // [x, y, z]            gravity vector
float ypr_current[3];     // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

// ================================================================
// ===            PRIMARY FUNCTION IMPLEMENTATION               ===
// ================================================================

//this goes in setup at some point, must be run first to get the 
//bearing of the submarine for the auto controls to work
void getBaseline()
{
  dpad_direction = NONE;

  //Set up and zero the position of the servos
  LR.attach(LRServo);
  LR2.attach(LRServo2);
  UD1.attach(PitchServo1);
  UD2.attach(PitchServo2);
  LR.write(LRPosition);
  LR.write(LRPosition);
  UD1.write(UDPosition);
  UD2.write(UDPosition);


  Serial.begin(9600);
  setupAccelerometer_sparkfun();
  delay(30000);//delay for the accelerometer to have consistent readings
  getAccelerometerVals_sparkfun();
  delay(100);//delay for the base values to be set
  getAccelerometerVals_sparkfun();
  BaseLine[0] = ypr_current[2];
  BaseLine[1] = ypr_current[0];
  BaseLine[2] = ypr_current[1];
}

//runs only the autonomous control portion of the code 
void runAutoControls()
{
  //updates the current vales for trajectory
  getAccelerometerVals_sparkfun();

  //update the values in the autonomous control system
  YawP = (int)((BaseLine[1] - ypr_current[0])*(180/3.14));
  YawI += (int)((BaseLine[1] - ypr_current[0])*(180/3.14));
  YawD = YawSlopePoints[NumSlopePoints - 1] - YawSlopePoints[0];
    
  PitchP = (int)((ypr_current[2] - BaseLine[0])*(180/3.14));
  PitchI += (int)((ypr_current[2] - BaseLine[0])*(180/3.14));
  PitchD = PitchSlopePoints[NumSlopePoints - 1] - PitchSlopePoints[0];

  LRchange = (int)((YawP * YawPCoefficient) + (YawI * YawICoefficient) + (YawD * YawDCoefficient));
  UDchange = (int)((PitchP * PitchPCoefficient) + (PitchI * PitchICoefficient) + (PitchD * PitchDCoefficient));

  //updates the slope points and gets rid of the oldest point
  for(int i = 0; i < (NumSlopePoints - 1); i++)
  {
    YawSlopePoints[i] = YawSlopePoints[i+1];
    PitchSlopePoints[i] = PitchSlopePoints[i+1];
  }
  
  YawSlopePoints[NumSlopePoints - 1] = (int)((BaseLine[1] - ypr_current[0])*(180/3.14));
  PitchSlopePoints[NumSlopePoints - 1] = (int)((ypr_current[2] - BaseLine[0])*(180/3.14));
  
  //Change the position of the servos
  LR.write(LRPosition + (LRchange));
  LR.write(LRPosition - (LRchange));
  UD1.write(UDPosition - (UDchange));
  UD2.write(UDPosition + (UDchange));
}

void runManualControls()
{
  dpad_direction = dpad_getDirection(); //update the dpad_direction variable

  //if a button has been pressed the switch statement determines the action to be taken 
  if((dpad_direction != NONE) && !reset)
  {
    DpadControl = 1;
    switch(dpad_direction){
      case UP: 
        UDchange = Override;
      break;
      case DOWN:
        UDchange = -Override;
      break;
      case LEFT:
        LRchange = Override;
      break;
      case RIGHT:
        LRchange = -Override;
      break;
      case CENTER:
        reset = true;
      break;
      case NONE:
        //Won't happen but will get rid of compiler warning
      break;
    }
  }
  //if the center button has been pressed and a peripheral button has been pressed
  else if((dpad_direction != NONE) && reset)
  {
    switch(dpad_direction){
      case UP: 
        BaseLine[0] += CourseCorrectionIncrement;
      break;
      case DOWN:
        BaseLine[0] -= CourseCorrectionIncrement;
      break;
      case LEFT:
        BaseLine[1] += CourseCorrectionIncrement;
      break;
      case RIGHT:
        BaseLine[1] -= CourseCorrectionIncrement;
      break;
      case CENTER:
        reset = false;
      break;
      case NONE:
        //Won't happen but will get rid of compiler warning
      break;
    }
  }
}


//runs the whole code with the built in switch for manual input 
//and a 50 delay at the end
void cruise()
{
  //updates the current vales for trajectory
  getAccelerometerVals_sparkfun();

  //update the values in the autonomous control system
  YawP = (int)((BaseLine[1] - ypr_current[0])*(180/3.14));
  YawI += (int)((BaseLine[1] - ypr_current[0])*(180/3.14));
  YawD = YawSlopePoints[NumSlopePoints - 1] - YawSlopePoints[0];
    
  PitchP = (int)((ypr_current[2] - BaseLine[0])*(180/3.14));
  PitchI += (int)((ypr_current[2] - BaseLine[0])*(180/3.14));
  PitchD = PitchSlopePoints[NumSlopePoints - 1] - PitchSlopePoints[0];

  //if the delay for the manual control input has expired the autonomous controls calculate change needed
  if(DpadControl > DpadControlZero)
  {
    LRchange = (int)((YawP * YawPCoefficient) + (YawI * YawICoefficient) + (YawD * YawDCoefficient));
    UDchange = (int)((PitchP * PitchPCoefficient) + (PitchI * PitchICoefficient) + (PitchD * PitchDCoefficient));
  }
    
  //Check if any buttons on the dpad have been pressed
  if (detectTransition(pin_dpad, 5, true)){
    dpad_direction = dpad_getDirection(); //update the dpad_direction variable
  }
  else dpad_direction = NONE;

  //if a button has been pressed the switch statement determines the action to be taken 
  if((dpad_direction != NONE) && !reset)
  {
    DpadControl = 1;
    switch(dpad_direction){
      case UP: 
        UDchange = Override;
      break;
      case DOWN:
        UDchange = -Override;
      break;
      case LEFT:
        LRchange = Override;
      break;
      case RIGHT:
        LRchange = -Override;
      break;
      case CENTER:
        reset = true;
      break;
      case NONE:
        //Won't happen but will get rid of compiler warning
      break;
    }
  }
  //if the center button has been pressed and a peripheral button has been pressed
  else if((dpad_direction != NONE) && reset)
  {
    switch(dpad_direction){
      case UP: 
        BaseLine[0] += CourseCorrectionIncrement;
      break;
      case DOWN:
        BaseLine[0] -= CourseCorrectionIncrement;
      break;
      case LEFT:
        BaseLine[1] += CourseCorrectionIncrement;
      break;
      case RIGHT:
        BaseLine[1] -= CourseCorrectionIncrement;
      break;
      case CENTER:
        reset = false;
      break;
      case NONE:
        //Won't happen but will get rid of compiler warning
      break;
    }
  }

  //increments the variable which controls the manual/autonomous delay
  DpadControl += 1;

  //updates the slope points and gets rid of the oldest point
  for(int i = 0; i < (NumSlopePoints - 1); i++)
  {
    YawSlopePoints[i] = YawSlopePoints[i+1];
    PitchSlopePoints[i] = PitchSlopePoints[i+1];
  }
  
  YawSlopePoints[NumSlopePoints - 1] = (int)((BaseLine[1] - ypr_current[0])*(180/3.14));
  PitchSlopePoints[NumSlopePoints - 1] = (int)((ypr_current[2] - BaseLine[0])*(180/3.14));
  
  //Change the position of the servos
  LR.write(LRPosition + (LRchange));
  LR.write(LRPosition - (LRchange));
  UD1.write(UDPosition - (UDchange));
  UD2.write(UDPosition + (UDchange));
  
  delay(50); //Might not be necessary
}




//call this function to read the analog pin and returns the dpad_direction variable
Direction dpad_getDirection(){
	Direction button; //Direction to return
    double value = analogRead(pin_dpad); //read the value on pin_dpad
    value = (value/1024)*4.84;           //convert to voltage
    //Serial.print(value);
    //Serial.print(" ");
    
    //up if the voltage is 0.75-1.25V
    if (value < 1.25 && value > 0.75){
        button = UP;
    }
    //down if the voltage is 4.75-5.25V
    else if (value < 5 && value > 4.75){
        button = DOWN;
    }
    //left if the voltage is 3.75-4.25V
    else if (value < 4.25 && value > 3.75){
        button = LEFT;
    }
    //right if the voltage is 2.75-3.25V
    else if (value < 3.25 && value > 2.75){
        button = RIGHT;
    }
    //center if the voltage is 1.75-2.25V
    else if (value < 2.25 && value > 1.75){
        button = CENTER;
 
      /*  // Code pasted from LCD
        //this sequence is meant to be called less frequently because it takes more time to execute
        //scroll to the next screen (the one to the right)
        //you can also call oledController->getPrev() to scroll left
        oledController->setIndex(oledController->getNext());
        //printHeaders() will update the yellow header and the arrows on the bottom
        headerChar++;
        //oledController->setHeaderChar(headerChar);
        oledController->printHeaders();
        oledController->refresh();
        // End Code pasted from LCD 
        
        */
        
    }
    
    else {
    	button = NONE;
    }
    
    return button;
}
//This function prints a variable of type Direction so we can read it easily
String printDirection(Direction d) {
    if (d == NONE) return "Dpad is not being pressed";
    else if (d == UP) return "Up";
    else if (d == DOWN) return "Down";
    else if (d == LEFT) return "Left";
    else if (d == RIGHT) return "Right";
    else if (d == CENTER) return "Center";
    else return "Dpad printing error";
}
//For switch debouncing!
//Waits a certain amount of time and returns if an edge was detected after the interval
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

// ================================================================
// ===              HELPER FUNCTION DEFINITIONS                 ===
// ================================================================

void setupAccelerometer_sparkfun(){
    // join I2C bus (I2Cdev library doesn't do this automatically)
    //Wire.begin(); //Might need this
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
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING); //Digital I/O pin 2
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
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


void getAccelerometerVals_sparkfun(){
      // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize);

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        //Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        
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
