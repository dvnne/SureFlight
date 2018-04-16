#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to these sensors */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

//float AccelMinX, AccelMaxX;
//float AccelMinY, AccelMaxY;
//float AccelMinZ, AccelMaxZ;
//
//float MagMinX, MagMaxX;
//float MagMinY, MagMaxY;
//float MagMinZ, MagMaxZ;

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
  
//  if (accelEvent.acceleration.x < AccelMinX) AccelMinX = accelEvent.acceleration.x;
//  if (accelEvent.acceleration.x > AccelMaxX) AccelMaxX = accelEvent.acceleration.x;
//  
//  if (accelEvent.acceleration.y < AccelMinY) AccelMinY = accelEvent.acceleration.y;
//  if (accelEvent.acceleration.y > AccelMaxY) AccelMaxY = accelEvent.acceleration.y;
//
//  if (accelEvent.acceleration.z < AccelMinZ) AccelMinZ = accelEvent.acceleration.z;
//  if (accelEvent.acceleration.z > AccelMaxZ) AccelMaxZ = accelEvent.acceleration.z;
//
//  if (magEvent.magnetic.x < MagMinX) MagMinX = magEvent.magnetic.x;
//  if (magEvent.magnetic.x > MagMaxX) MagMaxX = magEvent.magnetic.x;
//  
//  if (magEvent.magnetic.y < MagMinY) MagMinY = magEvent.magnetic.y;
//  if (magEvent.magnetic.y > MagMaxY) MagMaxY = magEvent.magnetic.y;
//
//  if (magEvent.magnetic.z < MagMinZ) MagMinZ = magEvent.magnetic.z;
//  if (magEvent.magnetic.z > MagMaxZ) MagMaxZ = magEvent.magnetic.z;
//
//  if ((millis() - lastDisplayTime) > 1000)  // display once/second
//  {
//    Serial.print("Accel Minimums: "); Serial.print(AccelMinX); Serial.print("  ");Serial.print(AccelMinY); Serial.print("  "); Serial.print(AccelMinZ); Serial.println();
//    Serial.print("Accel Maximums: "); Serial.print(AccelMaxX); Serial.print("  ");Serial.print(AccelMaxY); Serial.print("  "); Serial.print(AccelMaxZ); Serial.println();
//    Serial.print("Mag Minimums: "); Serial.print(MagMinX); Serial.print("  ");Serial.print(MagMinY); Serial.print("  "); Serial.print(MagMinZ); Serial.println();
//    Serial.print("Mag Maximums: "); Serial.print(MagMaxX); Serial.print("  ");Serial.print(MagMaxY); Serial.print("  "); Serial.print(MagMaxZ); Serial.println(); Serial.println();
//    lastDisplayTime = millis();
//  }
}
