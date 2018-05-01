#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

int m0 = 2; // Azimuth
int dir0 = 3;
int ena0 = 4;
int m1 = 5; // Polar
int dir1 = 6;
int ena1 = 7;
int toggle = 8;
float Pi = 3.14159;

// Positions (Units of Steps)
int pos0 = 0;
int pos1 = 0;

void setup(){
  Serial.begin(9600);
  // Output Pins
  pinMode(m0, OUTPUT);
  pinMode(dir0, OUTPUT);
  pinMode(ena0, OUTPUT);
  pinMode(m1, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(ena1, OUTPUT);
  // Input Pins
}

void loop(){
  // Delayed Events //
  static unsigned long time = 0;
  static unsigned long oldTime = 0;
  int T = 1000;
  time = millis() / T;
  if (time != oldTime){
    Serial.println("Timer Event");
    // Print Magnetometer 0
    // Print Magnetometer 1
    // Print Wind Speed
    oldTime = time;
  }
  // Serial Events //
  static int activeMotor = m0;
  if (Serial.available() > 0){
    // Motor Select
    if (Serial.peek() == 'm'){
      int input = Serial.parseInt();
      Serial.print("Motor Selected: ");
      Serial.println(input);
      if (input == 0) activeMotor = m0;
      if (input == 1) activeMotor = m1;
    }
    else {
      // Print Things
      Serial.print("Angle Selected: ");
      Serial.println(angle);
      Serial.print("Active Motor: ");
      Serial.print(activeMotor);
      // Move (if it's a good idea)
      int angle;
      angle = Serial.parseInt();
      // Determine if angle is valid
      if (isValidAngle(activeMotor, angle)){
        setPosition(activeMotor, angle);
      }
    }
  }
}

bool isValidAngle(int motor, int angle){
  if (motor == m0){
    if (angle <= 20 && angle >= 0) return true;
    else return false;
  }
  if (motor == m1){
    if (angle >= 0 && angle <= 360) return true;
    else return false;
  }
}

void setPosition(int motor, int angle){
  int currentPosition, steps;
  int dir = 1; // +1 forward | -1 reverse
  // Find current position
  if (motor = m0) currentPosition = pos0;
  else if (motor = m1) currentPosition = pos1;
  // Calculate steps required
  steps = calculateSteps(motor, currentPosition, angle);
  // Find path of least resistance - actually I don't feel like doing this now
  // Set direction
  // Reset current position
}

int calculateSteps(int motor, int currentPosition, int angle){
  int stepsPerRevolution;
  // These are technically magic numbers -- may fix later
  if (motor == m0) stepsPerRevolution = 47*400;
  else if (motor == m1) stepsPerRevolution = 4*400;
  // Convert selected angle to steps
  unsigned long newPosition;
  newPosition = angle * stepsPerRevolution / 360;
  Serial.print("Setting Motor ");
  Serial.print(motor);
  Serial.print(" to ");
  Serial.println(newPosition);
  // Calculate displacement
  int disp = newPosition - currentPosition;
  Serial.print("Displacement = ");
  Serial.println(disp);
  return disp
}