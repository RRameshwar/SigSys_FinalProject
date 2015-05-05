#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// Initialize the shield
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Begin motor 1 on the shield.  Usage below
// motor->setSpeed(n), 0 <= n <= 255
// motor->run(FORWARD/BACKWARD), changes direction
Adafruit_DCMotor *motor = AFMS.getMotor(1);
int sensorPin = A0;

int count = 0;

// PID constants that can be changed using Serial
// For example, use "P231" to change P to 231.
// NOTE: cannot set to negative yet
int kP;
int kI;
int kD;

// Scales P, I, and D into proper range so 0 <= p + i + d < 256
double scaleFactorP;
double scaleFactorI;
double scaleFactorD;

// Variables used for calculating P, I, and D
boolean runForward; // Controls overall direction of P, I, and D.  Change if going wrong way
double accumulator;    // Integrates the error, is negated when direction is switched
int lastRead;       // Used for D control and direction control 
int maxRead;        // Used for Proportional and Integral Control
double dt;          // Difference in time between calculations

int delayMils;
boolean printOn = true;

void setup(){
  // Pin and system setup code
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
  AFMS.begin();

  // Initialize values
  // Adjusted directly via Serial
  kP = 100; 
  kI = 100; 
  kD = 0; 

  lastRead = 0;
  maxRead = 500;
  accumulator = 0;
  runForward = true;

  // Defines delay time.  Higher dt makes for lower resolution
  dt = 1.0 / 50;
  delayMils = (int)(dt * 1000);
  
  // Change these so that the ranges input by the GUI range well.
  scaleFactorP = 1.0/400;
  scaleFactorI = 1.0/200000;
  scaleFactorD = -1.0/1000;
}

// Keeps reading the Serial values until it doesn't see a number.
// Helper function to understand.
// ONLY SEES INTEGERS.
int getSerialNum(){  
  int val = 0;
  int zeroByte = (int)('0'); // Uses as a base to convert a character to a number
  
  // Keep going as long as it sees an integer
  while (Serial.peek() >= (int)('0') && Serial.peek() <= (int)('9')){
    val = val * 10 + ((int)(Serial.read()) - zeroByte);
  }
  return val;
}

// This function is called every time a Serial Event happens
// It reacts to any P, I, or D changes sent through Serial.
// Must be in form (P/I/D)##### with no spaces and Natural Numbers
// e.g. P100, I40, D2
void serialEvent(){
  Serial.flush(); // Ignores its own Serial Writes

  char incomingByte = Serial.read(); // Get first Serial character to see what to do
  
  switch(incomingByte){
    case 'P':
      kP = getSerialNum();
      //Serial.print("kP set to ");
      Serial.println(kP, DEC);
    break;
    case 'I':
      kI = getSerialNum();
      //Serial.print("kI set to ");
      //Serial.println(kI, DEC);
    break;
    case 'D':
      kD = getSerialNum();
      //Serial.print("kD set to ");
      //Serial.println(kD, DEC);
    break;
    // Using a single lowercase p switches the print off.  Just convenience
    case 'p':
      switchPrint();
      break;
    //default: 
      //Serial.print("Input not understood: ");
      //Serial.println(incomingByte);
  }
  
}

// Get the understood "Error" of the system
// Error is equal to HIGHEST_READ - CURRENT_READ
// Highest read is set to current read if current read is greater
int error(int readVal){ 
  if (readVal > maxRead){    
    maxRead = readVal;
  }
  return maxRead - readVal;
}

// Switches motor direction and keeps memory of current direction
void switchDirection(){
  runForward = !runForward;
  if(runForward) 
    motor->run(FORWARD);
  else 
    motor->run(BACKWARD);
}

// Switches printing to serial on and off.
void switchPrint(){
  printOn = !printOn;
}

// Print function for pid.  Doesn't print if printOn is false
void printVals(double p, double i, double d){
  if(!printOn) return;

  int printP = (int) p;
  int printI = (int) i;
  int printD = (int) d;
  int drive = (int) (p + i + d);
//  Serial.print( "P: ");
//  Serial.print(p, 2);
//  Serial.print(" I: ");
//  Serial.print(i, 2);
//  Serial.print(" D: ");
//  Serial.print(d, 2);
//  Serial.print(" Total: ");
//  Serial.println(p + i + d, 3);
}

void loop(){
  int thisRead = analogRead(sensorPin);
  int diff = thisRead - lastRead;

  if(diff < 0){
    // Account for change in direction.
    // by switching direction, p is automatically reversed
    // The integration needs to be reversed
    switchDirection();
    accumulator *= -1;
    diff *= -1;
  }
  
  // Calculate PID
  int err = error(thisRead);
  double p = scaleFactorP * kP * err;
  /*
  Serial.print(kP);
  Serial.print(" ");
  Serial.print(err);
  Serial.print(" ");
  Serial.println(scaleFactorP);
  */
  

  accumulator += err * dt;
  double i = scaleFactorI * kI * accumulator;
  double d = scaleFactorD * kD * diff / dt;
  
  // Ensure D < 0
  if(d > 0)
    d *=-1;
    
  // Set Motor speed according to PID
  int drive = (int)(p + i + d);
  // printVals(p, i, d);
  constrain(drive, 0, 255);
  motor->setSpeed(drive);
  
  // Update and Loop
  lastRead = thisRead;
  //Serial.println(thisRead+', '+p+', '+i+', '+d);
  
  // Prints p, i, d, and Resistor Read in the form:
  // p,i,d,Resistor
  
  count = (count + 1) % 5;
  //Serial.println(p);
  if(count == 0){
    Serial.print(p, 2); // Print p with 2 decimal values
    Serial.print(",");
    Serial.print(i, 2);
    Serial.print(",");
    Serial.print(d, 2);
    Serial.print(",");
    Serial.println(thisRead);
  }
  
  delay(delayMils);
}
