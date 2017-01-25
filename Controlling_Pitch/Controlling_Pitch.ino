// Made by Selim Odok & Kait Tokarz
// Last Edited: 1/24/2017 (1485295439 BST)
// Poetry by Selim

double BLADE_ANGLE = 0;    // These are constants that are literally only ever gonna be used
double BLADE_LENGTH = 0;   // in calculateOpPitch
                           // We will have them once props makes a prop
double VELOCONSTANT = 0;   // This is the constant used to convert velocity units into the type
                           // needed for the calculateOpPitch equation

void pitchToAngle(double angle) {              // INPUT DEGREES 
    controlRads = angle * 2 * PI / 180;        // Turns the angle value into radians
    controlDist = (controlRads+1.396/-1.54);   // <TO BE FIXED> this is the relationship between rads and distance
    actuator_move_to(controlDist);             // sets linear actuator extension to what it needs to be

}

double calculateOpPitch(double velociraptor, double rpm) {
                           // takes in a velocity raptor and the RPM of the sub
                           // calculates using a really long formula I (Selim) derived one day
                           // It's probably not accurate but ¯\_(ツ)_/¯
  double tanComp = (velociraptor * VELOCONSTANT)/(rpm * BLADE_LENGTH);
  double OpPitchAngle = (atan(tanComp) * (180/(2*PI))) + BLADE_ANGLE;

  return OpPitchAngle;
}

// This is a poem for ben

// Ben.

// The design year had finally been over 
// But building the sub is much slower
// It'd probably be best
// when we start to test
// for Rothberg to somehow stay sober 
