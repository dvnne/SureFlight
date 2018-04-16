#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
float x;
//float offx;
//float rx;
float y;
//float offy;
//float ry;

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


void setup(void) 
{
  Serial.begin(9600);
  
  /* Initialise the sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}

void loop(void) 
{
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
  delay(3000);
}
