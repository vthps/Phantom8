#ifndef CONTROLS_H
#define CONTROLS_H

// ================================================================
// ===                    HELPER FUNCTIONS                      ===
// ================================================================
void setupAccelerometer_sparkfun();
void getAccelerometerVals_sparkfun();   
bool detectTransition(int pin, int sample_time_ms, bool is_rising);


#endif /* CONTROLS_H */