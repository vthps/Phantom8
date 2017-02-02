void setup() {
    rpm_sensor_setup(); //Setup RPM Sensors
    setup_battery_monitor(); //Setup battery voltage monitoring
    pressure_sensor_setup(); //Setup Pressure sensors
    //lcd_setup(); //Setup LCD
    //servo_setup(); //Setup servos
    datalogger_setup(); //Setup datalogging
    controls_setup(); // Setup accelerometer
    
}

void loop() {
    //Read Sensors
    double a = rpm_calculate(); //Read rpm into double variable
    double b = measure_battery();



    //Make Calculations
        //Selim's Equation
    




    //Update servos and actuator
        //Update servos
        //Update actuator
}
