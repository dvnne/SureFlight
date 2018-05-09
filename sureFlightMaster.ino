#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

float Pi = 3.141592653589793238462643383279502884197169; // youre welcome davonne
bool calibrated = 0;
int guiState = 0;
bool enabled = 0;
bool zeroed = 0;

int fanPin = 42;

/* Set motor pins */
// All motor values and pins are azimuth,polar
int motor[] = {5,2};
int dir[] = {6,3};
int enable[] = {7,4};
int limitSwitch = 12;
int calibrateButton = 19;

int pos[] = {0,0};
int gearRatio[] = {47,4};
int stepsPerRotation = 400;
int motordelay = 10;


/* Wind Speed pins and constants*/
int anemometerPin = A0;
float anemometerVoltage = 0;
float windSpeed = 0;

float voltageConversionConstant = .004882814; //This constant maps 0 to 1023, to actual voltage, which ranges from 0V to 5V
float voltageMin = .4; // Mininum output voltage from anemometer in V.
float windSpeedMin = 0; // Wind speed in meters/sec corresponding to minimum voltage
float voltageMax = 2.0; // Maximum output voltage from anemometer in V.
float windSpeedMax = 32.4; // Wind speed in meters/sec corresponding to maximum voltage

/* Wind Vane pins */
int windVane[] = {22, 24, 26, 28, 30, 32, 34};
float windDir = 0;


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
  enableMotors();

  pinMode(limitSwitch, INPUT);
  pinMode(calibrateButton,INPUT);
  pinMode(fanPin,INPUT);
  digitalWrite(fanPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(calibrateButton), calibratePolar, RISING);

  for (int i=0; i < 7; i++){
    pinMode(windVane[i], INPUT);
  }

}


void loop() {
  //if(Serial.available()>0) {
    guiState=Serial.parseInt();


    if ((guiState < 4602) && (guiState > 999)) {
      
      if (calibrated == 1) {
        manualMotors();
      } else {
        uncalibrated();
      }
    
    
    } else if(guiState == 5000) { // load mode deactivated by GUI 
        enableMotors();
    } else if(guiState == 5001) { // load mode activated by GUI 
        disableMotors();

    } else if(guiState == 6001) { // zero mode activated by GUI

      if (calibrated == 1) {
        zeroMotors();
        Serial.println(6000); // tells GUI done zeroing
        guiState = 6000;
      } else {
        uncalibrated();
      }

    } else if (guiState == 7001){ //AAD activated by GUI
      if (calibrated == 1) {
        AAD();
      } else {
        uncalibrated();
      }

    }
    //Serial.println(guiState);
    //else {
          //Serial.println(6543);
    //}
}

void uncalibrated() {
  Serial.println(8000);
  guiState = 8000;
}

void AAD() {
  int moveStat = 1;
  if (zeroed == 0) {
    zeroMotors();
  }
	getWindDirection();
	getWindSpeed();

	int azimuthAngle = atan2(windSpeed,500) * 5;
	int polarAngle = windDir;

	moveStat = moveAngles(azimuthAngle, polarAngle);
  if (moveStat == 1) {
    guiState = 7000;
    Serial.println(7000); // tells GUI done with AAD
  }

}

void manualMotors(){
  int moveStat = 1;
  if (zeroed == 0) {
    zeroMotors();
  }
  int selectedMotor = guiState%2;
  int selectedAngle = (guiState/10) - 100;
  if (selectedMotor == 0) {
    moveStat = moveAngles(selectedAngle, pos[1]);
  }
  else {
    moveStat = moveAngles(pos[0], selectedAngle);
  }

  if ((moveStat == 1)) {
    guiState = 9999;
    Serial.println(9999);
//  } else if ((moveStat ==1) && (selectedMotor ==0)){
//        guiState = 9998;
//    Serial.println(9998);
  }
}

////////////////////////////
/* Motor Helper Functions */
////////////////////////////

void disableMotors() {
  enabled = 0;
  digitalWrite(enable[0],HIGH);
  digitalWrite(enable[1],HIGH);
	// command both motors to disable
}

void enableMotors() {
  enabled = 1;
	digitalWrite(enable[0],LOW);
	digitalWrite(enable[1],LOW);
}

int calcSteps(int m, int angle){

  int steps;
  float rotation = angle/360.; // fraction of arc to rotate

  steps = rotation * stepsPerRotation;
  steps = steps * gearRatio[m];
//  Serial.print("Setting steps to ");
//  Serial.println(steps);
  return steps;
}

int setPosition(int m, int curPosition, int position) {
  int disp = position - curPosition; // number of steps needed
  // define sign: false = polar clockwise, down azimuth
  bool sign;
  if (disp > 0) sign = false;
  else if (disp < 0) sign = true;
  // Rotate motor
//  Serial.print("Setting Position -- Displacement = ");
//  Serial.println(disp);
  if (disp != 0) {
    if (false) { //m == 0 and digitalRead(limitSwitch) == HIGH) {
      pos[0] = 0;
      Serial.println(9000);
      return 0;
    } else{
      step(m, abs(disp),sign);
    }
  }
  return 1;
}

int moveAngles(int azimuthAngle, int polarAngle) {
//  Serial.print("Setting angles to ");
//  Serial.print(azimuthAngle);
//  Serial.print(", ");
//  Serial.println(polarAngle);
  int motostat = 1;
	if (azimuthAngle <= 20 && polarAngle <= 360) {
    int newAzimuth = calcSteps(0, azimuthAngle); // units of steps
    motostat = setPosition(0, calcSteps(0,pos[0]), newAzimuth);
    if (motostat == 1) {
      pos[0] = azimuthAngle;
    } else {
      pos[0] = 0;
      return 0;
    }
    int newPolar = calcSteps(1, polarAngle);
//    Serial.print("Steps calculated: ");
//    Serial.println(newPolar);
    setPosition(1, calcSteps(1,pos[1]),newPolar);
    pos[1] = polarAngle;
  }
  return 1;
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
	//CHECK MOTOR DIRECTIONS /////////////////////////////////
  enableMotors();
	while (digitalRead(limitSwitch) == LOW) {
		step(0, 1, HIGH);
		
	}
	step(1,pos[1],HIGH);
	pos[0] = 0;
	pos[1] = 0;
  zeroed = 1;
}

void calibratePolar() {
//  Serial.println("calibrated");
  calibrated = 1;
  pos[1] = 0;
}

/////////////////////////////
/* Sensor Helper Functions */
/////////////////////////////

void getWindDirection() {
	// read input from other arduino

  unsigned int n = 0;
    for (int i=6; i > -1; i--){
        int b = digitalRead(windVane[i]);
        Serial.print(b);
        if (b){
            float t = pow(2, i);
            t += 0.5;
            t = int(t);
            n += t;
        }
    }
    
   windDir = map(n,0,127,0,360);
//   Serial.print("winddir: ");
//   Serial.println(windDir);

}

void getWindSpeed() {
	anemometerVoltage = analogRead(anemometerPin) * voltageConversionConstant;
	//Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
	if (anemometerVoltage <= voltageMin){
		//Check if voltage is below minimum value. If so, set wind speed to zero.
 		windSpeed = 0;
	} else {
		//For voltages above minimum value, use the linear relationship to calculate wind speed.
  		windSpeed = (anemometerVoltage - voltageMin)*windSpeedMax/(voltageMax - voltageMin);
	}
// Serial.print("windspeed: ");
// Serial.println(windSpeed);
}

