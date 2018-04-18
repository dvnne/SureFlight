#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

int m1 = 2; // Azimuth
int dir1 = 3;
int m2 = 4; // Polar
int dir2 = 5;
int button = 6;
float Pi = 3.14159;

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

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(button, INPUT);
  // Initialize sensor
  if(!mag.begin()) {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}

void loop(){
  // Timer variables
  int T = 500;
  unsigned long time = millis()/T;
  static unsigned long oldTime = 0;
  // Do Button Things
  if (digitalRead(button == HIGH)){
    Serial.println("HIGH");
    digitalWrite(dir1, HIGH);
    step(m2, 400);
    digitalWrite(dir2, HIGH);
    step(m1, 400);
    digitalWrite(dir2, LOW);
    step(m1, 400);
  }
  // Read Magnetometer 1
  getSensorEvent();
  float heading = calculateHeading();
  if (oldTime != time) {
    Serial.println("Event");
    oldTime = time;
    printCompassHeading(heading);
  }
  // Print Mag 2 Data
  if (Serial1.available() > 0) {
    float incomingByte = Serial1.parseFloat();
    Serial.print("I recieved: ");
    Serial.println(incomingByte);
  }
}

void step(int motorPin, int steps){
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

void getSensorEvent(){
  sensors_event_t event; 
  mag.getEvent(&event);
  x = event.magnetic.x;
  y = event.magnetic.y;
  x = (x-offx) * ry / rx;
  y = y - offy;
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