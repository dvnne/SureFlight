#include <Wire.h>

float Pi = 3.141592653589793238462643383279502884197169; // youre welcome davonne

/* Set motor pins */
// All motor values and pins are azimuth,polar
int motor[] = {4,2};
int dir[] = {5,3};
int enable[] = {9,8};
int limitswitch = 6;

int pos[] = {0,0};
int gearRatio[] = {47,4};
int stepsPerRotation = 400;
int motordelay = 10;

void setup(void) {
  Serial.begin(9600);

  pinMode(motor[0], OUTPUT);
  pinMode(motor[1], OUTPUT);
  pinMode(dir[0], OUTPUT);
  pinMode(dir[1], OUTPUT);
  digitalWrite(dir[0], LOW); // Initialize stepper in forward direction
  digitalWrite(dir[1],LOW);
  pinMode(enable[0], OUTPUT);
  pinMode(enable[1],OUTPUT);
  digitalWrite(enable[0],LOW);
  digitalWrite(enable[1],LOW);

  calibrateMagnetometer();
  
}

void loop() {

	if (Serial.available() > 0) {
    int azimuthAngle = Serial.parseInt();
    int polarAngle = Serial.parseInt();
    Serial.print("Azimuth, Polar: ");
    Serial.print(azimuthAngle);
    Serial.print(", ");
    Serial.println(polarAngle);
    moveAngles(azimuthAngle,polarAngle);
  }
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

void calibrateMagnetometer() {
  int xmax = 0;
  int xmin = 1000;
  int ymax = 0;
  int ymin = 1000;
  for (int i=0; i<stepsPerRotation, i++) {
    getSensorEvent();
    Serial.print(x);
    Serial.print(",");
    Serial.println(y);
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

