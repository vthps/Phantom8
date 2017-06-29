#include "Arduino.h"
#include "common.h"

void datalogger_setup() {
    Serial3.begin(9600); //Datalogger connected to Serial3 on D14/D15.
}
