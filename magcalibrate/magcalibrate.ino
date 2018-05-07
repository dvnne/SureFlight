#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
float x;
float y;
float xmax, ymax;
float xmin, ymin;

float offx = 0;
float offy = 0;
float rx = 0;
float ry = 0;

// Button
int button = 12;
bool isCalibrating = false; // should be true while button is being held
// Useful Constants
float Pi = 3.1415926535;
// Output
int outPins[] = {1, 2, 3, 4, 5, 6, 7};


void setup(void) {
  Serial.begin(9600); 
  /* Initialise the sensor */
  if(!mag.begin()) {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}

void loop(void) {
  // Time
  static unsigned long time = 0;
  static unsigned long oldTime = 0;
  int T = 500;
  time = millis() / T;
  // Event Actions
  getMagnetometerEvent();
  float heading = calculateHeading();
  // SEND HEADING TO MEGA
  int sendVal = map(heading,0,360,0,127);
  write(sendVal);
  
  if (time != oldTime) { // Run every T milliseconds
    Serial.print("Compass Heading: ");
    Serial.println(heading); // Print compass heading
    Serial.print("Button Reading");
    Serial.println(digitalRead(button));
    Serial.print("7 bit number: ");
    Serial.println(sendVal);
    oldTime = time;
  }
  // BUTTON EVENTS
  if (digitalRead(button) == HIGH) { 
    isCalibrating = true; // button is being held
    getCalibrationData();
  }
  if (digitalRead(button) == LOW && isCalibrating == true) { // button released
    isCalibrating = false;
    calibrateMagnetometer(); // do the math
  }
}

float getCalibrationData() {
  getMagnetometerEvent(); // get replace global x, y with raw data
  if (x > xmax) xmax = x;
  if (x < xmin) xmin = x;
  if (y > ymax) ymax = y;
  if (y < ymin) ymin = y;
}

void calibrateMagnetometer() {
  Serial.print("{xmax, ymax, xmin, ymin}");
  Serial.println(xmax);
  Serial.println(ymax);
  Serial.println(xmin);
  Serial.println(ymin);
  offx = (xmax + xmin)/2;
  offy = (ymax + ymin)/2;
  rx = (xmax - xmin)/2;
  ry = (ymax - ymin)/2;
}

void getMagnetometerEvent() {
  sensors_event_t event; 
  mag.getEvent(&event);
  x = event.magnetic.x;
  y = event.magnetic.y;
}

float calculateHeading() {
  // Apply corrections
  x = (x-offx) * ry / rx;
  y = y - offy;
  // Calculate the angle of the vector y,x
  float heading = (atan2(y,x) * 180) / Pi;
  // Normalize to 0-360
  if (heading < 0) {
    heading = 360 + heading;
  }
  return heading;
}


void write(unsigned int n){
    int bin;
    for (int i=0; i< 7; i++){
        bin = (n / pow(2, i));
        bin = (int) bin;
        bin = bin % 2;
        digitalWrite(outPins[i], bin);
//        Serial.print(bin);
    }
//    Serial.print('\n');
}
