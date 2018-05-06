#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

float Pi = 3.141592653589793238462643383279502884197169; // you're welcome davonne
int button = 1;
bool calibrated = 0;
bool launch = 0;
int recValue; //received command from GUI
int load = 0; //load flag

/* Set motor pins */
// All motor values and pins are azimuth,polar
int motor[] = {2,4};
int dir[] = {3,5};
int enable[] = {8,9};
int limitswitch = 6;

int pos = {0,0};
int gearRatio = {47,4};
int stepsPerRotation = 400;
int motordelay = 10;

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
float x, y;
float offx = 0;
float offy = 0;
float rx = 1;
float ry = 1;

/* Wind Speed pins and constants*/
int anemometerPin = A0;
float anemometerVoltage = 0;
float windSpeed = 0;

float voltageConversionConstant = .004882814; //This constant maps 0 to 1023, to actual voltage, which ranges from 0V to 5V
float voltageMin = .4; // Mininum output voltage from anemometer in V.
float windSpeedMin = 0; // Wind speed in meters/sec corresponding to minimum voltage
float voltageMax = 2.0; // Maximum output voltage from anemometer in V.
float windSpeedMax = 32.4; // Wind speed in meters/sec corresponding to maximum voltage



void setup(void) {
  Serial.begin(9600);

  //Setup Motors
  pinMode(motor[0], OUTPUT);
  pinMode(motor[1], OUTPUT);
  pinMode(dir[0], OUTPUT);
  pinMode(dir[1], OUTPUT);
  digitalWrite(dir[0], LOW); // Initialize stepper in forward direction
  digitalWrite(dir[1],LOW);
  pinMode(enable[0], OUTPUT);
  pinMode(enable[1],OUTPUT);
  disableMotors();
  // pinMode(button, INPUT);

  // Initialize sensor
  if(!mag.begin()) {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}


void loop() {
  if(Serial.available()>0)
  {
    recValue=Serial.parseInt();

    if (recValue < 3602){
      int selectedMotor = recValue%2;
      int selectedAngle = recValue/10;
      //move selected motor to selected angle
      Serial.println(9999);//tells MATLAB done moving

    }
    
  }else if(recValue == 4001)            // load mode activated by MATLAB      
     { 
      load = 1;
      disableMotors;
     }
   else if(recValue == 4000)          // load mode deactivated  by MATLAB 
     { 
      load = 0;
      enableMotors;
     }
//    else if(recValue == 5001)  { // calibration mode activated
//		calibrated = 1;
//		calibrateMagnetometer();
//		zeroMotors();
//    Serial.println(5000);//tells MATLAB done calibrating
	}
	elseif(recValue == 6001){ // CHANGE TO USER INPUT ZERO ALL MOTORS
		if (calibrated) {
			enableMotors();
			zeroMotors();
     Serial.println(6000);//tells MATLAB done zeroing
//		} else {
//			/////////////////////DISPLAY ERROR
//		}
	}}
	elseif (recValue == 7001){ //AAD has been pressed in GUI
	  AAD(); 
   Serial.println(7000);//tells MATLAB done with AAD
	}
  


}

void AAD() {
	getWindDirection();
	getWindSpeed();

	int azimuthAngle = 0;
	int polarAngle = 0;

	moveAngles(azimuthAngle, polarAngle);
  launch = 1;

}

////////////////////////////
/* Motor Helper Functions */
////////////////////////////

void disableMotors() {
  digitalWrite(enable[0],HIGH);
  digitalWrite(enable[1],HIGH);
	// command both motors to disable
}

void enableMotors() {
	digitalWrite(enable[0],LOW);
	digitalWrite(enable[1],LOW);
}

int getPosition(int m, int angle){
  int steps;
  float rotation = angle/360.; // fraction of arc to rotate
  Serial.print("Setting Rotation to ");
  Serial.println(rotation);
  steps = (angle * stepsPerRotation)/360;
  steps = steps * gearRatio[m];
  return steps;
}

void setPosition(int m, int curPosition, int position) {
  int disp = position - curPosition; // number of steps needed
  // define sign: false = polar clockwise, down azimuth
  bool sign;
  if (disp > 0) sign = false;
  else if (disp < 0) sign = true;
  // Rotate motor
  Serial.print("Setting Position -- Displacement = ");
  Serial.println(disp);
  if (disp != 0) step(m, abs(disp),sign);
}

void moveAngles(int azimuthAngle, int polarAngle) {
	if (azimuthAngle <= 20 && polarAngle <= 360) {
    int newAzimuth = getPosition(0, azimuthAngle); // units of steps
    setPosition(0, pos[0], newAzimuth);
    pos[0] = newAzimuth;

    int newPolar = getPosition(1, polarAngle);
    setPosition(1, pos[1],newPolar);
    pos[1] = newPolar;
  }
}

void step(int m, int steps, int direction){
	digitalWrite(dir[m],direction);
  delay(motordelay);
  for (int i=0; i < steps; i++){
    digitalWrite(motor[m], LOW);
    delay(motordelay);
    digitalWrite(motor[m], HIGH);
    delay(motordelay);
  }
  digitalWrite(motor[m], LOW);
}

void zeroMotors() {
	//CHECK MOTOR DIRECTIONS /////////////////////////////////////////////////////////////////
	if (calibrated == 0) {
		//DISPLAY ERROR TO USER
		return;
	}
	digitalWrite(enable[0],LOW);
  digitalWrite(enable[1],LOW);
	while (digitalRead(limitswitch) == LOW) {
		step(0, 1, HIGH);
		pos[0] = 0;
	}
	step(0,pos[1],HIGH);
	pos[1] = 0;
}

/////////////////////////////
/* Sensor Helper Functions */
/////////////////////////////

void calibrateMagnetometer() {
	int xmax = 0;
	int xmin = 1000;
	int ymax = 0;
	int ymin = 1000;
	for (int i=0; i<stepsPerRotation, i++) {
		getSensorEvent();
		step(polar,1,LOW);
		if (x > xmax) xmax = x;
		if (x < xmin) xmin = x;
		if (y > ymax) ymax = y;
		if (y < ymin) ymin = y;
	}

	offx = (xmax + xmin) / 2;
	offy = (ymax + ymin) / 2;
	rx = (xmax - xmin) / 2;
	ry = (ymax - ymin) / 2;

	getSensorEvent();
	polarpos = calculateHeading();
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

void getWindDirection() {
	// read input from other arduino
}

void getWindSpeed() {
	anemometerVoltage = analogread(anemometerPin) * voltageConversionConstant;
	//Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
	if (anemometerVoltage <= voltageMin){
		//Check if voltage is below minimum value. If so, set wind speed to zero.
 		windSpeed = 0;
	} else {
		//For voltages above minimum value, use the linear relationship to calculate wind speed.
  		windSpeed = (anemometerVoltage - voltageMin)*windSpeedMax/(voltageMax - voltageMin);
	}
}

