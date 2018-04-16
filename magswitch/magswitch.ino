#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

// MAGNETOMETER 1 VALUES:
int pin = 2;
float offx = -2.23;
float offy = -5.09;
float rx = 60.32;
float ry = 61.64;

// MAGNETOMETER 2 VALUES:
/*
  pin = 3;
  offx = -7.775;
  offy = -2.815;
  rx = 60.135;
  ry = 61.635;
*/

void setup(void) 
{
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  Serial.begin(9600);
  Serial.println("Hello");
  if (!mag.begin()){
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!"); 
  }
}

void loop(void) 
{
  Serial.println("Loop");
  static int activeMag = 1;
  int x, y;

  if( Serial.available() ){
    int m = Serial.parseInt();
    if( m != activeMag){
      if( m == 1 ){
        digitalWrite(2, HIGH);
        digitalWrite(3, LOW);
      }
      if( m == 2){
        digitalWrite(2, LOW);
        digitalWrite(3, HIGH);
      }
      delay(10);
      if (!mag.begin()){
        Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
      }
    }
  }

  /* Get a new sensor event */ 
  sensors_event_t event; 
  mag.getEvent(&event);
  x = event.magnetic.x;
  y = event.magnetic.y;

  x = (x-offx) * ry / rx;
  y = y - offy;
  
  float Pi = 3.14159;
  
  // Calculate the angle of the vector y,x
  float heading = (atan2(y,x) * 180) / Pi;
  
  // Normalize to 0-360
  if (heading < 0)
  {
    heading = 360 + heading;
  }
//  Serial.print("Compass Heading: ");
  Serial.println(heading);
  // delay(500);
}
