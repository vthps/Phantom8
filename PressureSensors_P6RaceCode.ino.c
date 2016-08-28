/*End four variables*/
double slopeConver = 0.29995;  //Theoretical slope conversion calculated from Px In and Ouput on data sheet (bar/volt)
double voltConver = .0048828;  //Convert analog signal to volts = 5V/1023
double densityH2O = 1000;      //Density of water = 1000 kg/m3
double gravity = 9.831;        //Garvitational acceleration = 9.831 m/s2
double knotConver = 1.9438;    //Convert m/s to knot
double vel = 0;                //Velocity (knot) from two pressure values

//Pressure Sensors
int pinStaticPressure = A2; //Pin for sensor two input
int pinStagPressure = A1;   //Pin for sensor one input
double stagPressure = 0;   //Value of pressure (pa) from sensor one --> stagnation pressure
double staticPressure = 0; //Value of pressure (pa) from sensor two --> static pressure
double depthStaticPressure = 0;       //Cacluated depth (meter) from sensor one
double depthStagPressure = 0;       //Cacluated depth (meter) from sensor one

void getPressure()
{
  //Read in analog (voltage)
  units1 = analogRead(pinStagPressure);
  units2 = analogRead(pinStaticPressure);
  //Convert units from analog to volts
  stagPressure = units1*voltConver;
  staticPressure = units2*voltConver;
  //Convert voltage to pressure in bars
  stagPressure = stagPressure*slopeConver;
  staticPressure = staticPressure*slopeConver;
  //Convert bars to pa
  stagPressure = stagPressure*100000;
  staticPressure = staticPressure*100000;
}

void calcDepth()
{
  //Calculate depth in meters
  //depthStagPressure = stagPressure/(densityH2O*gravity);
  //depthStaticPressure = staticPressure/(densityH2O*gravity);

  depthStagPressure = (units1 / 1024.0) * 5.0;
   depthStagPressure = (((((((depthStagPressure + 0.0107) / 1.5303)) - .0296) / .0096)) - 23.0) / 12.0;  //  /12.0
  depthStaticPressure = (units2 / 1024.0) * 5.0;
   depthStaticPressure = (((((((depthStaticPressure + 0.0107) / 1.5303)) - .0296) / .0096)) - 23.0) / 12.0;  //  /12.0

    if(depthStaticPressure < 0) depthStaticPressure = 0;
  if(depthStagPressure < 0)   depthStagPressure = 0;
  stagPressure = depthStagPressure / 33.4552565551; // in psi
  staticPressure = depthStaticPressure / 33.4552565551; //  in psi
}

void calcSpeed()
{
  //(2*(Pressure_Stagnation-Pressure_Static))/densityH2O
  double radicand = (2*(stagPressure - staticPressure))/densityH2O;
  //Make sure do not take square root of negative number
  if (radicand < 0) { 
    Speed = 0;
  } 
  else {
    Speed = sqrt(radicand);
  }
}

double getSpeed()
{
  calcSpeed();
  return Speed; 
}