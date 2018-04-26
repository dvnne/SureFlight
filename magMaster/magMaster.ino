#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

// Set motor pins
int m1 = 2;
int m2 = 4;
int dir = 3;
float Pi = 3.14159;
int button = 6;

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
float x;
float y;

//MAGNETOMETER 1 VALUES:
//float offx = -2.23;
//float offy = -5.09;
//float rx = 60.32;
//float lry = 61.64;

//MAGNETOMETER 2 VALUES:
float offx = -7.775;
float offy = -2.815;
float rx = 60.135;
float ry = 61.635;

// SETUP //
void setup(void) {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(dir, OUTPUT);
  digitalWrite(dir, LOW); // Initialize stepper in forward direction 
  pinMode(button, INPUT);
  // Initialize sensor
  if(!mag.begin()) {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}

// LOOP //
void loop(void) {
  // CONSTANTS //
  static int motorPosition = 0; // Stepper starts at position 0 (steps)
  // Timer variables
  int T = 500;
  unsigned long time = millis()/T;
  static unsigned long oldTime = 0;
  // SERIAL EVENTS //
  // Print data from Arduino 2
  if (Serial1.available() > 0) {
    float incomingByte = Serial1.parseFloat();
    Serial.print("I recieved: ");
    Serial.println(incomingByte);
  }
  // Set stepper position from serial monitor
  if (Serial.available() > 0) {
    int inputAngle = Serial.parseInt();
    Serial.print("inputAngle: ");
    Serial.println(inputAngle);
    if (inputAngle <= 20) {
      int newPosition = getPosition(inputAngle); // units of steps
      setPosition(motorPosition, newPosition);
      motorPosition = newPosition;
    }
  }
  if (digitalRead(button) == HIGH){
    for (int i=0, i < 400, i++){
      step(m2, 1);
    }
  }
  // Read Magnetometer 1
  getSensorEvent();
  float heading = calculateHeading();
  // Delayed Events
  if (oldTime != time) {
    oldTime = time;
    printCompassHeading(heading);
  }
}

// Serial.print("Compass Heading: ");
void printCompassHeading(float heading) {
  Serial.println(heading);
}

int getPosition(int angle){
  int steps;
  int gearRatio = 47;
  int stepsPerRotation = 400;
  float rotation = angle/360.; // fraction of arc to rotate
  Serial.print("Setting Rotation to ");
  Serial.println(rotation);
  steps = (angle * stepsPerRotation)/360;
  steps = steps * gearRatio;
  return steps;
}

void setPosition(int curPosition, int position) {
  int disp = position - curPosition; // number of steps needed
  // define sign: true - forward, false - reverse
  bool sign;
  if (disp > 0) sign = true;
  else if (disp < 0) sign = false;
  // Set rotation direction
  if (!sign) digitalWrite(dir, HIGH);
  else digitalWrite(dir, LOW);
  // Rotate motor
  Serial.print("Setting Position -- Displacement = ");
  Serial.println(disp);
  if (disp != 0) step(m1, abs(disp));
}

void getSensorEvent(){
  sensors_event_t event; 
  mag.getEvent(&event);
  x = event.magnetic.x;
  y = event.magnetic.y;
  x = (x-offx) * ry / rx;
  y = y - offy;
}

float calculateHeading(){
  float heading = (atan2(y,x) * 180) / Pi; // Calculate the angle of the vector y,x
  // Normalize to 0-360
  if (heading < 0) heading = 360 + heading;
  return heading;
}

void step(int motor, int n){
  unsigned long start = millis();
  delay(10);
  for (int i=0; i < n; i++){
    digitalWrite(motor, LOW);
    delay(10);
    digitalWrite(motor, HIGH);
    delay(10);
  }
  digitalWrite(motor, LOW);
}
