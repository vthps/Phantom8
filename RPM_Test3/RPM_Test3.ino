#include "RPM_DRIVER"

void setup() {
  // put your setup code here, to run once:
  rpm_sensor_setup;
  
}

void loop() {
  // put your main code here, to run repeatedly:
  rpm_current = rpm_calculate();
  Serial.println(rpm_current);
  pause(2); // wait to run the loop again
}
