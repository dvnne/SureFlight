#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to these sensors */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

/* This script prints raw accelerometer and magnetometer data to the serial output,
   where it can be saved as a .csv and analyzed separately */


float ax,ay,az;
float mx,my,mz;

long lastDisplayTime;

void setup(void) 
{
  Serial.begin(9600);
  
  /* Initialise the accelerometer */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  /* Initialise the magnetometer */
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  lastDisplayTime = millis();
}

void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t accelEvent; 
  sensors_event_t magEvent; 
  
  accel.getEvent(&accelEvent);
  mag.getEvent(&magEvent);
  
  ax = accelEvent.acceleration.x;
  ay = accelEvent.acceleration.y;
  az = accelEvent.acceleration.z;

  mx = magEvent.magnetic.x;
  my = magEvent.magnetic.y;
  mz = magEvent.magnetic.z;
  if ((millis() - lastDisplayTime) > 1000)  // display once/second
  {
    Serial.print(ax); Serial.print(",");
    Serial.print(ay); Serial.print(",");
    Serial.print(az); Serial.print(",");

    Serial.print(mx); Serial.print(",");
    Serial.print(my); Serial.print(",");
    Serial.println(mz);
  }

}
