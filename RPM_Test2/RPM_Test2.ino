#define INPUT_PIN_1 18    // Analog input pins to use
#define INPUT_PIN_2 19
#define SENSITIVITY 400   // Lowest number considered to be "HIGH"
#define POLL_LENGTH 500  // Length of polling period
#define RPM_LEN 10

int sensitivity = 400;

int ctr;
int activeSensor;

double prevReadings[RPM_LEN];

int startPoll;

void setup()
{
  activeSensor = INPUT_PIN_1;
  
  ctr = 0;
  startPoll = millis();
  
  Serial.begin(9600);
}

void loop()
{
  if (analogRead(activeSensor) > SENSITIVITY) {
    ctr++;
    
    while(analogRead(activeSensor) > SENSITIVITY);
    
    if (activeSensor == INPUT_PIN_1)
      activeSensor = INPUT_PIN_2;
    else
      activeSensor = INPUT_PIN_1;
  }
  
  if (millis() > (startPoll + POLL_LENGTH)) {
    for (int i = 0; i < (RPM_LEN - 1); i++)
      prevReadings[i] = prevReadings[i+1];
    
    prevReadings[RPM_LEN - 1] = ((double)ctr / ((double)POLL_LENGTH / 1000)) * 60;
    
    ctr = 0;
    startPoll = millis();
  }
  
  // Handle serial requests
  if (Serial.available()) {
    // Flush serial buffer for all except a single char
    while (Serial.available() > 1)
      Serial.read();
    
    // Read the request char, handle request
    char rx = Serial.read();
    switch(rx) {
      // Run sensor calibration
      case 'C':
        calibrationTest();
        break;
      // Send current RPM
      case 'R':
        Serial.println(getRPM());
        break;
    } 
  }
}

// Determine max and min values of sensor, set HIGH/LOW threshold
void calibrationTest() {
  int startTime = millis();
  int low = analogRead(INPUT_PIN_1), hi = analogRead(INPUT_PIN_1);
  
  // Run for 10 seconds
  while (millis() < startTime + 10000) {
    int rdOne = analogRead(INPUT_PIN_1);
    int rdTwo = analogRead(INPUT_PIN_2);
    
    if (rdOne < low)
      low = rdOne;
    if (rdTwo < low)
      low = rdTwo;
      
    if (rdOne > hi)
      hi = rdOne;
    if (rdTwo > hi)
      hi = rdTwo;
  }
  
  sensitivity = low + ((hi - low) / 2);
  Serial.println(low);
  Serial.println(hi);
  Serial.println(sensitivity);
}

// Average RPM using previous 10 values
double getRPM() {
  double toReturn;
  for (int i = 0; i < RPM_LEN; i++) {
    toReturn += prevReadings[i];
  }
    
  return (toReturn / RPM_LEN);
}

