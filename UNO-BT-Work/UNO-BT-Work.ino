#include<SoftwareSerial.h>
#include <Stepper.h>

int ENA = 8;
int ENB = 9;

int IN1 = 10;
int IN2 = 11;

int IN3 = 12;
int IN4 = 13;

int MotorASpeed = 255;
int MotorBSpeed = 255;

int state;
String temp;
SoftwareSerial bt(A5, A4); /* (Rx,Tx) */	

const int trigPin = 7;
const int echoPin = 8;

// MoveCondition
bool forward;
bool backward;
bool left;
bool right;
bool front = true;
bool back = true;

int trigPinF = 7;    // TRIG pin
int echoPinF = 6;    // ECHO pin

int trigPinB = 4;    // TRIG pin
int echoPinB = 5;    // ECHO pin

const int stepsPerRevolution = 1000;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, A0, A1, A2, A3, 8);

// Mode Control
//https://web.stanford.edu/class/cs101/bits-bytes.html
bool isAutoMove;

bool checkGround;

void setup() {
  bt.begin(9600);
  Serial.begin(9600);

  myStepper.setSpeed(60);

  pinMode(trigPinF, OUTPUT);
  pinMode(echoPinF, INPUT);
  pinMode(trigPinB, OUTPUT);
  pinMode(echoPinB, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {

  if ((forward || isAutoMove) && front) {
    chechFrontDistance();
  }
  else if (backward) {
    chechBackDistance();
  }

  if(bt.available() > 0){ // Checks whether data is comming from the serial port
    temp = bt.read(); // Reads the data from the serial port
    bt.println(temp);
    if (temp.length() > 0)
      state = temp.toInt();

    switch (state) {

      case 87: backward = false; // W forward
                 forward = true;
                 break;
      case 119:forward = false; // w forward stop
                 backward = false;
                 isAutoMove = false;
                 break;

      case 83: forward = false; // S backward
                 backward = true;
                 break;
      case 115: forward = false; // s backward stop
                 backward = false;
                 break;

      case 65: right = false; // A left
                 left = true;
                 break;
      case 97: right = false; // a left stop
                 left = false;
                 break;

      case 68: left = false; // D right
                 right = true;
                 break;
      case 100: left = false; // d right stop
                 right = false;
                 break;

      case 80:  MotorASpeed, MotorBSpeed = 255;
                break;
      case 112:  MotorASpeed, MotorBSpeed = 180;
                break;

      case 113: isAutoMove = true;
               break;
      case 81: isAutoMove = false;
               break;

      case 71: groundCheck();
               break;

      case 111: Serial.println("o");
               break;

    }

    if ((forward || isAutoMove) && !backward && !left && !right && front && !checkGround)
    {
      moveForward();
    }
    else if (!forward && backward && !left && !right && back && !checkGround) 
    {
      moveBackward();
    }
    else if ((!forward && !backward && left && !right) || (isAutoMove && left && !right) && !checkGround)
    {
      moveLeft();
    }
    else if ((!forward && !backward && !left && right) || (isAutoMove && !left && right) && !checkGround) 
    {
      moveRight();
    }
    else 
    {
      stop();
    }
 }

}

void groundCheck() {
  checkGround = true;
  stop();
  myStepper.step(stepsPerRevolution);
  delay(1000);
  Serial.println("G");
  // delay(10000);
  delay(10000);
  myStepper.step(-stepsPerRevolution);
  myStepper.step(-stepsPerRevolution);
  myStepper.step(-stepsPerRevolution);
  myStepper.step(-stepsPerRevolution);
  myStepper.step(-stepsPerRevolution);
  state = 0;
  checkGround = false;
}

void chechFrontDistance() {
  float duration_us, distance_cm;
  digitalWrite(trigPinF, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinF, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(echoPinF, HIGH);
  // calculate the distance
  distance_cm = 0.017 * duration_us;

  if (distance_cm < 5) {
    stop();
  }
}

void chechBackDistance() {
  float duration_us, distance_cm;
  digitalWrite(trigPinB, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinB, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(echoPinB, HIGH);
  // calculate the distance
  distance_cm = 0.017 * duration_us;

  if (distance_cm < 5) {
    stop();
  }
}

void moveRight() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite (ENA, MotorASpeed);
    analogWrite (ENB, MotorBSpeed);
}

void moveLeft() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite (ENA, MotorASpeed);
    analogWrite (ENB, MotorBSpeed);
}

void moveForward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite (ENA, MotorASpeed);
    analogWrite (ENB, MotorBSpeed);
}

void moveBackward() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite (ENA, MotorASpeed);
    analogWrite (ENB, MotorBSpeed);
}

void stop() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite (ENA, 0);
    analogWrite (ENB, 0);
}