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

// Magnetometers
/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
float x;
float y;

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
  // Initialize sensor
  if(!mag.begin()) {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}

void loop(){
  static unsigned long time = 0;
  static unsigned long oldTime = 0;
  int T = 100;
  time = millis() / T;
  // Delayed Events //
  if (time != oldTime){
    // Read Magnetometer 0
    getSensorEvent();
    printxy();
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
      if (input == 0){
        activeMotor = m0;
        Serial.print("Motor Selected: ");
        Serial.println(input);
      }
      if (input == 1) {
        activeMotor = m1;
        Serial.print("Motor Selected: ");
        Serial.println(input);
      }
    }
    else {
      int angle = Serial.parseInt();
      // Print Things
      Serial.print("Angle Selected: ");
      Serial.println(angle);
      Serial.print("Active Motor Pin: ");
      Serial.println(activeMotor);
      // Set angle if appropriate
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
  int currentPosition, dirPin, steps;
  // Set current pins and varaibles
  if (motor == m0) {
    currentPosition = pos0;
    dirPin = dir0;
  }
  else if (motor == m1) {
    currentPosition = pos1;
    dirPin = dir1;
  }
  steps = calculateSteps(motor, currentPosition, angle);
  // Set direction
  if (steps >= 0) {
    digitalWrite(dirPin, HIGH);
  }
  else {
    digitalWrite(dirPin, LOW);
  }
  // Move to position
  stepMotor(motor, abs(steps));
  // Reset current position
  currentPosition += steps;
  if (motor == m0) pos0 = currentPosition;
  else if (motor == m1) pos1 = currentPosition;
  Serial.print("Motor 0 Position: ");
  Serial.println(pos0);
  Serial.print("Motor 1 Position: ");
  Serial.println(pos1);
}

void stepMotor(int motorPin, int steps){
  unsigned long start = millis();
  delay(10);
  for (int i=0; i < steps; i++){
    digitalWrite(motorPin, LOW);
    delay(10);
    digitalWrite(motorPin, HIGH);
    delay(10);
  }
  digitalWrite(motorPin, LOW);
}

int calculateSteps(int motor, int currentPosition, int angle){
  int stepsPerRevolution;
  // These are technically magic numbers -- may fix later
  if (motor == m0) stepsPerRevolution = 47*400;
  else if (motor == m1) stepsPerRevolution = 4*400;
  // Convert selected angle to steps
  float newPosition;
  newPosition = angle / 360.;
  newPosition = newPosition * stepsPerRevolution;
  newPosition = (unsigned int) (newPosition + 0.5);
  Serial.print("Setting Motor (Pin no)");
  Serial.print(motor);
  Serial.print(" to ");
  Serial.println(newPosition);
  // Calculate displacement
  int disp = newPosition - currentPosition;
  Serial.print("Displacement = ");
  Serial.println(disp);
  return disp;
}

void getSensorEvent(){
  sensors_event_t event; 
  mag.getEvent(&event);
  x = event.magnetic.x;
  y = event.magnetic.y;
  // x = (x-offx) * ry / rx;
  // y = y - offy;
}

// Serial.print("Compass Heading: ");
void printCompassHeading(float heading) {
  Serial.println(heading);
}

float calculateHeading(){
  float heading = (atan2(y,x) * 180) / Pi; // Calculate the angle of the vector y,x
  // Normalize to 0-360
  if (heading < 0) heading = 360 + heading;
  return heading;
}

void printxy(){
  Serial.print(x);
  Serial.print('\t');
  Serial.println(y);
}