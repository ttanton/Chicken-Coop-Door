// Photoresistor Chicken Coop Door
// 7/23/16

#include <Stepper.h>
#define STEPS 200 // number of steps on the motor

// General Settings
int cycleDelayt = 5; // Delay of system cycle in seconds
int night = 80;
int daylight = 300;
int twilight = daylight - 1; // Currently not used

// Pin Locations
int in1Pin = 12;          // stepper controller in1
int in2Pin = 11;          // stepper controller in2
int in3Pin = 10;          // stepper controller in3
int in4Pin = 9;           // stepper controller in4
int topReedPin = 3; // digital pin tied to top reed switch
int bottomReedPin = 2; // digital pin tied to bottom reed switch
int photocellPin = 0;     // the cell and 10K pulldown are connected to a0

// System Variables
int cycleDelay = cycleDelayt * 1000; // Delay converted to ms
int lightState; // 0 night, 1 twilight, 2 daylight
int prevLightState;
int doorState; // state of door-  1 for down, 2 for up, and 0 for neither
int photocellReading;     // the analog reading from the sensor divider
int lastPhotocellReading = 0;     // the analog reading from the sensor divider

Stepper motor(STEPS, in1Pin, in2Pin, in3Pin, in4Pin);

void setup(void) {

  Serial.begin(9600);

  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  motor.setSpeed(150);

}

void getDoorState() {
  // Read Door State and return 1 for down, 2 for up, and 0 for neither
  int top = digitalRead(topReedPin);
  int bottom = digitalRead(bottomReedPin);

  if (top) {
    doorState = 2;
  }
  else if (bottom) {
    doorState = 1;
  }
  else {
    doorState = 0;
  }

}

void getLight() {
  // Get light reading from photoresistor

  int a = analogRead(photocellPin);
  int tempState;

  if (a <= night) {
    tempState = 0;
  }
  else if (a <= twilight) {
    tempState = 1;
  }
  else {
    tempState = 2;
  }

  // If system uptime is less than 8s - make prevLightState equal to lightState
  if (millis() < 8000) {
    prevLightState = tempState;
    lightState = tempState;
  }
  else {
    // Delay 15 minutes if nightfall
    //if(lightState != 0 && tempState == 0){
    //  delay(15*60*1000);
    //}
    prevLightState = lightState;
    lightState = tempState;
  }
}

void doDoorUp() {
  while (doorState != 2) {
    motor.step(100);
    getDoorState();
  }

  // Kill Motor Signals
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, LOW);
  digitalWrite(in4Pin, LOW);
}

void doDoorDown() {
  int cycles = 0;
  while (doorState != 1 && prevLightState != 0) {
    motor.step(-100);
    getDoorState();
    cycles ++;
    if (cycles > 50)
    motor.step(2000);
  }

  // Kill Motor Signals
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, LOW);
  digitalWrite(in4Pin, LOW);
}


void loop(void) {

  getDoorState();

  if (doorState == 0) {
    doDoorDown();
  }


  // Assume doorState now = down

  getLight();

  if (lightState == 0) {
    doDoorDown();
  }

  if (lightState == 2) {
    doDoorUp();
  }

  delay(cycleDelay);
}

