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
int accumulator;    // Integrates the error, is negated when direction is switched
int lastRead;       // Used for D control and direction control 
int maxRead;        // Used for Proportional and Integral Control
double dt;          // Difference in time between calculations

int delayMils;
boolean printOn = true;

void setup(){
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
  AFMS.begin();

  // Initialize values
  kP = 100; 
  kI = 0; 
  kD = 0; 
  lastRead = 0;
  maxRead = 500;
  accumulator = 0;
  runForward = true;
  dt = 1.0 / 1000;
  delayMils = (int)(dt * 1000);
  scaleFactorP = 1.0/1000;
  scaleFactorI = 1.0/1000;
  scaleFactorD = -1.0/1000;
}

int getSerialNum(){
  int val = 0;
  int zeroByte = (int)('0');
  while (Serial.peek() >= (int)('0') && Serial.peek() <= (int)('9')){
    val = val * 10 + ((int)(Serial.read()) - zeroByte);
  }
  Serial.flush();
  return val;
}

void parseSerial(){
  char incomingByte = Serial.read();
  
  switch(incomingByte){
    case 'P':
      kP = getSerialNum();
      Serial.println("kP set to " + kP);
    break;
    case 'I':
      kI = getSerialNum();
      Serial.println("kI set to " + kP);
    break;
    case 'D':
      kD = getSerialNum();
      Serial.println("kD set to " + kP);
    break;
    case 'p':
      switchPrint();
    default: 
      Serial.print("Input not understood: ");
      Serial.println(incomingByte);
  }
  
}

int error(int readVal){
  // Get error
  if (readVal > maxRead){
    
    maxRead = readVal + 1;
  }
  return maxRead - readVal;
}

void switchDirection(){
  runForward = !runForward;
  if(runForward) 
    motor->run(FORWARD);
  else 
    motor->run(BACKWARD);
}

void switchPrint(){
  printOn = !printOn;
}

void printVals(double p, double i, double d){
  if(!printOn) return;

  int printP = (int) p;
  int printI = (int) i;
  int printD = (int) d;
  int drive = (int) (p + i + d);
  Serial.print( "P: " + printP);
  Serial.print(" I: " + printI);
  Serial.print(" D: " + printD);
  Serial.println(" Total: " + drive);
}

void loop(){
  if(Serial.available())
    parseSerial();

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
  
  int err = error(thisRead);
  double p = kP * err * scaleFactorP;
  accumulator += err * dt;
  double i = kI * accumulator * scaleFactorI;
  double d = kD * diff / dt * scaleFactorD;
    
  int drive = (int)(p + i + d);
  printVals(p, i, d);
  motor->setSpeed(drive);
  
  lastRead = thisRead;
  delay(delayMils);
}
