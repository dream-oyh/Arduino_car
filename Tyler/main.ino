#include<SoftwareSerial.h>
#include<AFMotor.h>
#include<Servo.h>
void servo_init();
void HC_init();
int getDistance();
void move();
void stop();
void manmode();
int getDistance_fro();
int getDistance_custom(int pos);
void limit_move(int speed, int de, int dir);
void autoturn();
void automode();
void runmode();
void set_servo(int pos);
// SoftwareSerial BTSerial(9,2); //RX,TX
SoftwareSerial BTSerial(9, 10);
// motor define
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);
// define servo
Servo myservo;

// const define
#define LEFT_POS 50
#define RIGHT_POS 150
#define CENTER_POS 100
#define OK_DIST 20  // safe distance (TODO)
#define MOTOR_SPEED 180 
#define HC_TRIG_PIN A1 // from board to HC_SR04
#define HC_ECHO_PIN A0 // from HC_SR04 to board
#define SERVO_DELAY 300 
#define TURN_OVER 100 // turn over time (TODO)
#define TURN_POS 40 // turn angle time (TODO)

#define MAN 0
#define AUTO 1
#define SERVO_PIN 10
// global variable

char mode = 'S'; // stop mode
int systemmode = MAN;
int pos = CENTER_POS;

void setup(){
  servo_init();
  motor1.setSpeed(MOTOR_SPEED);
  motor2.setSpeed(MOTOR_SPEED);
  motor3.setSpeed(MOTOR_SPEED);
  motor4.setSpeed(MOTOR_SPEED);

  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
  BTSerial.begin(9600);
}

void set_servo(int pos){
  myservo.write(pos);
  delay(SERVO_DELAY);
}

void servo_init(){
  myservo.attach(SERVO_PIN);
  set_servo(CENTER_POS);
}

void HC_init(){
  pinMode(HC_ECHO_PIN, INPUT);
  pinMode(HC_TRIG_PIN, OUTPUT);
}
int getDistance(){
  digitalWrite(HC_TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(HC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(HC_TRIG_PIN, LOW);

  float duration = pulseIn(HC_ECHO_PIN, HIGH);
  return (duration / 2) / 29.1;
}


void move(int speed, int dir){
  switch (dir)
  {
  case 1: // forward:
    /* code */
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  break;
  case 2: //backward:
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  break;
  case 3: // left turn
  motor1.run(BACKWARD);
  motor4.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  break;
  case 4: // right turn
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor1.run(FORWARD);
  motor4.run(FORWARD);
  break;
  }

  motor1.setSpeed(speed);
  motor2.setSpeed(speed);
  motor3.setSpeed(speed);
  motor4.setSpeed(speed);
}
void stop(){
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
void manmode(){
    switch (mode)
    {
    case 'F':
      move(MOTOR_SPEED, 1);
      break;
    case 'B':
      move(MOTOR_SPEED, 2);
      break;
    case 'L':
      move(MOTOR_SPEED - 50, 3);
      break;
    case 'R':
      move(MOTOR_SPEED - 50, 4);
      break;
    case 'S':
      stop();
      break;
    }
}
int getDistance(){

  digitalWrite(HC_TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(HC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(HC_TRIG_PIN, LOW);
  
  float duration = pulseIn(HC_ECHO_PIN, HIGH);
  return (duration/2) / 29.1;
}
int getDistance_custom(int pos){
  set_servo(pos);
  int distance =  getDistance();
  delay(50);
  return distance;
}
void limit_move(int speed, int de, int dir){
  move(speed,dir);
  delay(de);
  stop();
}
void autoturn(){
  int leftDistance = getDistance_custom(LEFT_POS);
  int rightDistance = getDistance_custom(RIGHT_POS);
  if (leftDistance < OK_DIST && rightDistance < OK_DIST){
    limit_move(MOTOR_SPEED, TURN_OVER, 4); // turn right
  } else if (leftDistance > OK_DIST && rightDistance < OK_DIST)
  {
    limit_move(MOTOR_SPEED, TURN_POS, 3); // turn left
  } else if (rightDistance > OK_DIST){
    limit_move(MOTOR_SPEED, TURN_POS, 4); // turn right
  }
  set_servo(CENTER_POS);
}

void automode(){
  if (mode=='S'){
    stop();
    set_servo(CENTER_POS);
  }else{
    int dist = getDistance_custom(CENTER_POS);
    if (dist<=OK_DIST){
      stop();
      autoturn();
    }
    else{
      move(MOTOR_SPEED, 1); // forward
    }
  }
}
void runmode(){
    switch (mode)
    {
    case 'M':
      systemmode = MAN;
      stop();
      mode = 'S';
    case 'A':
      systemmode = AUTO;
    }
    if (systemmode){
      automode();
    } else{
      manmode();
    }
}
void loop(){
  if (BTSerial.available() > 0){
    mode = BTSerial.read();
    }
    runmode();
}