/* datalogger.ino
 * Serial0 is used for debug information.
 * Serial3 is connected to the datalogger.
 * Writing to Serial3 will log data onto the SD card, and writing to Serial0 will let you see output without having it saved to the SD card.
 * 
 * Pin connections:
 *     Openlog  |  Arduino
 *       GRN    |    A10
 *       RXI    |    TX3
 *       TX0    |    RX3
 *       VCC    |    5V
 *       GND    |    not connected
 *       BLK    |    GND
 */


//pins
int pin_datalogger_reset = A10;

//local variables
int i=0;


//setup() function. Runs once
void setup(){
    pinMode(pin_datalogger_reset, OUTPUT);
    
    Serial.begin(9600);  //Serial0 (serial window through USB)
    Serial3.begin(9600); //Serial3 is for the datalogger

    Serial.println("Resetting datalogger...");
    datalogger_reset(pin_datalogger_reset);
    Serial.println("datalogger reset");
    
    delay(1000);
}

//loop() function. Runs forever
void loop(){
    byte byteRead;
    if (Serial.available()){
        if (Serial.read() == 'l'){
            Serial.println("Attempting to print datalogger files to Serial0...");
            datalogger_printFilesToSerial();
            Serial.println("Finished printing.");
        }
    }
}



//This function prints all of the text files on the SD card to Serial0
//It is assumed that the datalogger is on Serial3
void datalogger_printFilesToSerial(){
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
void datalogger_reset(int pin){
    digitalWrite(pin, LOW);
    delay(100);
    digitalWrite(pin, HIGH);
}




