
void setup() {
    rpm_sensor_setup(); //Setup RPM Sensors
    setup_battery_monitor(); //Setup battery voltage monitoring
    pressure_sensor_setup(); //Setup Pressure sensors
    //lcd_setup(); //Setup LCD
    datalogger_setup(); //Setup datalogging
    oled_setup(); //Setup lcd screen
    
    getBaseline(); //Controls Setup. Needs 30 seconds when submarine is lined up to race
}

void loop() {
    //Read Sensors
    double a = rpm_calculate(); //Read rpm into double variable
    double b = measure_battery();

    cruise(); //Start auto controls with manual input available


    //Make Calculations
        //Selim's Equation
    




    //Update servos and actuator
        runAutoControls(); //Run autopilot for controls
        //Update actuator


    //If buoy is released {
        //Buoy released = false; }
        
}
