#include <helper_3dmath.h>
#include <MPU6050_6Axis_MotionApps20.h>
//#include <MPU6050_9Axis_MotionApps41.h>
#include <MPU6050.h>

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

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
// ===                    HELPER FUNCTIONS                      ===
// ================================================================
void setupAccelerometer_sparkfun();
void getAccelerometerVals_sparkfun();   
    

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    Serial.begin(9600);
    setupAccelerometer_sparkfun();
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
		  getAccelerometerVals_sparkfun();
      
			Serial.print("ypr_current\t");
			Serial.print(ypr_current[0] * 180/M_PI);
			Serial.print("\t");
			Serial.print(ypr_current[1] * 180/M_PI);
			Serial.print("\t");
			Serial.println(ypr_current[2] * 180/M_PI);
      
}



// ================================================================
// ===              HELPER FUNCTION DEFINITIONS                 ===
// ================================================================

void setupAccelerometer_sparkfun(){
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








