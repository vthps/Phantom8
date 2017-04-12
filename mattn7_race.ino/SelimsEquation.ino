// Made by Selim Odok & Kait Tokarz
// Last Edited: 1/24/2017 (1485295439 BST)
// Poetry by Selim

const double BLADE_ANGLE = 0;    // These are constants that are literally only ever gonna be used
const double BLADE_LENGTH = 0;   // in calculateOpPitch
                          // We will have them once props makes a prop
const double VELOCONSTANT = 0;   // This is the constant used to convert velocity units into the type
                          // needed for the calculateOpPitch equation
const double VELOTHRESHOLD = 0;  // This is the threshold decided upon to be what triggers the switch
                          // in modes for the propeller. We can calculate this or set it manually
boolean thrustMode = true; // This is the variable that controls which propeller mode we're in
                          // if it's true we're in thrust mode, if it's false we're in efficiency
                          // we start the race in thrust mode

void pitchToAngle(double angle) {              // INPUT DEGREES
   controlRads = angle * 2 * PI / 180;        // Turns the angle value into radians
   controlDist = (controlRads+1.396/-1.54);   // <TO BE FIXED> this is the relationship between rads and distance
   actuator_move_to(controlDist);             // sets linear actuator extension to what it needs to be

}

double calculateOpPitch(double velo, double rpm, boolean thrustMode) {  // takes in a velocity and the RPM of the sub
   double tanComp = (velo * VELOCONSTANT)/(rpm * BLADE_LENGTH);        // calculates using a really long formula I (Selim) derived one day
   double OpPitchAngle = (atan(tanComp) * (180/(2*PI))) + BLADE_ANGLE; // It's probably not accurate but ¯\_(ツ)_/¯
   /* THIS ALL NEEDS ANOTHER SECTION IN ORDER TO DIFFERENTIATE BETWEEN THRUST MODE AND EFFICIENCY MODE
    * UNFORTUNATELY YOU CAN ONLY FIND THAT BY TESTING THE PROPELLER
    * AND WE DON'T HAVE ONE
    * I DON'T KNOW WHY WE DON'T HAVE ONE WHEN OUR PROPS TEAM IS SITTING ON ITS HANDS
    * BUT WE DON'T
    * AND SINCE WE'RE PROBABLY GONNA END UP USING THE OLD PROPS
    * WE SHOULD REALLY TEST THOSE
    * BUT THAT WOULD MAKE TOO MUCH SENSE WOULDN'T IT
    * :angry:
    */
   return OpPitchAngle;    
}

void propellerModeControl(double velo, double VELOTHRESHOLD, boolean thrustMode){
   if (thrustMode){ // if the propeller is in high thrust mode
       if(velo > (VELOTHRESHOLD * 1.1)){ // and if we've got a pretty good speed going
           thrustmode = false;           // then we should switch to efficiency mode
       }
   }else{ // if the propeller is in high efficiency mode
       if(velo < (VELOTHRESHOLD * 0.9)){ // and if we fall too low below the threshold
           thrustmode = true;            // GOTTA GO FAST!
       }
   }
}

// This is a poem for Caleb

// Caleb.

// I understand when you say
// that to our fate we've resigned
// but take your negativity
// and stick it up your behind
