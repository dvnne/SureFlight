float Pi = 3.141592653589793238462643383279502884197169; // youre welcome davonne
bool calibrated = 0;
bool launch = 0;
int guiState = 0;
bool enabled = 0;
bool zeroed = 0;



/* Set motor pins */
// All motor values and pins are azimuth,polar
int motor[] = {2,5};
int dir[] = {3,6};
int enable[] = {4,7};
int limitSwitch = 18;
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
int windVane[] = {30, 32, 34, 36, 38, 40, 42, 44};
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
  disableMotors();

  pinMode(limitSwitch, INPUT);
  pinMode(calibrateButton,INPUT);
  attachInterrupt(digitalPinToInterrupt(calibrateButton), calibratePolar, RISING);

  for (int i=0; i < 8; i++){
    pinMode(windVane[i], INPUT);
  }

}


void loop() {
  if(Serial.available()>0) {
    guiState=Serial.parseInt();

    if (guiState < 3602) {
      if (calibrated) {
        manualMotors();
      } else {
        uncalibrated();
      }
    }
    
    } else if(guiState == 4000) { // load mode activated by GUI 
        disableMotors();
    } else if(guiState == 4001) { // load mode deactivated by GUI 
        enableMotors();

    } else if(guiState == 6001) { // zero mode activated by GUI

      if (calibrated) {
        zeroMotors();
        Serial.println(6000); // tells GUI done zeroing
      } else {
        uncalibrated();
      }

    } else if (guiState == 7001){ //AAD activated by GUI
      if (calibrated) {
        AAD(); 
        Serial.println(7000); // tells GUI done with AAD
      } else {
        uncalibrated();
      }

    }


}

void uncalibrated() {
  Serial.println(8000);
}

void AAD() {
  if (zeroed == 0) {
    zeroMotors();
  }
	getWindDirection();
	getWindSpeed();

	int azimuthAngle = 0;
	int polarAngle = 0;

	moveAngles(azimuthAngle, polarAngle);
  launch = 1;

}

void manualMotors(){
  if (zeroed == 0) {
    zeroMotors();
  }
  int selectedMotor = guiState%2;
  int selectedAngle = guiState/10;
  if (selectedMotor == 0) {
    moveAngles(selectedAngle, pos[1]);
  }
  else {
    moveAngles(pos[0], selectedAngle);
  }
  Serial.println(9999);
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
  if (disp != 0) {
    if (m == 0 and digitalRead(limitSwitch) == HIGH) {
      pos[0] = 0;
      Serial.println(9000);
      return;
    } else{
      step(m, abs(disp),sign);
    }
  }
}

void moveAngles(int azimuthAngle, int polarAngle) {
	if (azimuthAngle <= 20 && polarAngle <= 360) {
    int newAzimuth = calcSteps(0, azimuthAngle); // units of steps
    setPosition(0, pos[0], newAzimuth);
    pos[0] = newAzimuth;

    int newPolar = calcSteps(1, polarAngle);
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
	//CHECK MOTOR DIRECTIONS /////////////////////////////////
  enableMotors();
	while (digitalRead(limitSwitch) == LOW) {
		step(0, 1, HIGH);
		
	}
	step(0,pos[1],HIGH);
	pos[0] = 0;
	pos[1] = 1;
  zeroed = 1;
}

void calibratePolar() {
  calibrated = 1;
  pos[1] = 0;
}

/////////////////////////////
/* Sensor Helper Functions */
/////////////////////////////

void getWindDirection() {
	// read input from other arduino

  unsigned int n = 0;
    for (int i=7; i > -1; i--){
        int b = digitalRead(windVane[i]);
        Serial.print(b);
        if (b){
            float t = pow(2, i);
            t += 0.5;
            t = int(t);
            n += t;
        }
    }
   windDir = n;

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
}

