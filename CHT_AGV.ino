#include "QGPMaker_MotorShield.h"
#include <Ultrasonic.h>

#define ALARM 8
#define UltraTrig 13
#define UltraEcho 12
#define LeftInfrared 6
#define RightInfrared 7
const int dangerThresh = 15; //580;   // 10cm × 58

int distance;
// Ultrasonic ultrasonic(UltraTrig, UltraEcho);
int speed=50;  //default 200
int Left_val=0;
int Right_val=0;
String RecvWord;

QGPMaker_MotorShield AFMS = QGPMaker_MotorShield();

QGPMaker_DCMotor *DCMotor_2 = AFMS.getMotor(2);
QGPMaker_DCMotor *DCMotor_4 = AFMS.getMotor(4);
QGPMaker_DCMotor *DCMotor_1 = AFMS.getMotor(1);
QGPMaker_DCMotor *DCMotor_3 = AFMS.getMotor(3);

void forward() {
  DCMotor_1->setSpeed(speed);
  DCMotor_1->run(FORWARD);
  DCMotor_2->setSpeed(speed);
  DCMotor_2->run(FORWARD);
  DCMotor_3->setSpeed(speed);
  DCMotor_3->run(FORWARD);
  DCMotor_4->setSpeed(speed);
  DCMotor_4->run(FORWARD);
}

void turnLeft() {
  DCMotor_1->setSpeed(speed);
  DCMotor_1->run(BACKWARD);
  DCMotor_2->setSpeed(speed);
  DCMotor_2->run(BACKWARD);
  DCMotor_3->setSpeed(speed);
  DCMotor_3->run(FORWARD);
  DCMotor_4->setSpeed(speed);
  DCMotor_4->run(FORWARD);
}

void turnRight() {
  DCMotor_1->setSpeed(speed);
  DCMotor_1->run(FORWARD);
  DCMotor_2->setSpeed(speed);
  DCMotor_2->run(FORWARD);
  DCMotor_3->setSpeed(speed);
  DCMotor_3->run(BACKWARD);
  DCMotor_4->setSpeed(speed);
  DCMotor_4->run(BACKWARD);
}

void moveLeft() {
  DCMotor_1->setSpeed(speed);
  DCMotor_1->run(BACKWARD);
  DCMotor_2->setSpeed(speed);
  DCMotor_2->run(FORWARD);
  DCMotor_3->setSpeed(speed);
  DCMotor_3->run(BACKWARD);
  DCMotor_4->setSpeed(speed);
  DCMotor_4->run(FORWARD);
}

void moveRight() {
  DCMotor_1->setSpeed(speed);
  DCMotor_1->run(FORWARD);
  DCMotor_2->setSpeed(speed);
  DCMotor_2->run(BACKWARD);
  DCMotor_3->setSpeed(speed);
  DCMotor_3->run(FORWARD);
  DCMotor_4->setSpeed(speed);
  DCMotor_4->run(BACKWARD);
}

void backward() {
  DCMotor_1->setSpeed(speed);
  DCMotor_1->run(BACKWARD);
  DCMotor_2->setSpeed(speed);
  DCMotor_2->run(BACKWARD);
  DCMotor_3->setSpeed(speed);
  DCMotor_3->run(BACKWARD);
  DCMotor_4->setSpeed(speed);
  DCMotor_4->run(BACKWARD);
}

void stopMoving() {
  DCMotor_1->setSpeed(0);
  DCMotor_1->run(RELEASE);
  DCMotor_2->setSpeed(0);
  DCMotor_2->run(RELEASE);
  DCMotor_3->setSpeed(0);
  DCMotor_3->run(RELEASE);
  DCMotor_4->setSpeed(0);
  DCMotor_4->run(RELEASE);
}

void AvoidObstacle(){
  // distance = ultrasonic.read(); //不加參數就是輸出CM，可用read(INC)輸出英寸
  Serial.println("ultrasonic:");
  Serial.println(distance);
  int movetime=1000;
  if (distance<dangerThresh){
    stopMoving();
    delay(30);
    if(!CheckLeftEmpty(movetime)){
      moveRight(); // back to origin location
      delay(movetime);
      if(!CheckRightEmpty(movetime)){
        moveLeft(); // back to origin location
        delay(movetime);
        turnRight();  //如果前面沒路，迴轉180度
        delay(50);  
      }
    }
    forward();
  }
}

bool CheckLeftEmpty(int movetime){
    moveLeft();
    delay(movetime);
    stopMoving();
    // distance = ultrasonic.read();
    if (distance<dangerThresh){
      return false;
    }else{
      forward();
      delay(movetime*2);
      moveRight();
      delay(movetime);
    }
    return true;
}

bool CheckRightEmpty(int movetime){
    moveRight();
    delay(movetime);
    stopMoving();
    // distance = ultrasonic.read();
    if (distance<dangerThresh){
      return false;
    }else{
      forward();
      delay(movetime*2);
      moveLeft();
      delay(movetime);
    }
}

void ReceiveAlarm(){
  if (Serial.available()){
    RecvWord = Serial.readStringUntil('\n');
    if(RecvWord=="Unknown Person"){
      stopMoving();
      digitalWrite(ALARM,HIGH);
      delay(1200);
      digitalWrite(ALARM,LOW);
    }else if(RecvWord=="Potential Intrusion"){
      stopMoving();
      delay(1500);
    }
    Serial.println(RecvWord);
  }
}

void setup() {
  // put your setup code here, to run once:
  // 設定馬達
  Serial.begin(115200);
  Serial.println("Hello, I am running...");
  AFMS.begin(50);

  // 設定紅外線sensor輸出腳位
  pinMode(LeftInfrared, INPUT);  // 左紅外線觸發腳設定成「對於arduino的輸入」
  pinMode(RightInfrared, INPUT);   // 右紅外線觸發腳設定成「對於arduino的輸入」

  //設定避障輸入輸出腳位
  // pinMode(UltraTrig, OUTPUT);  
  // pinMode(UltraEcho, INPUT);

  //設定蜂鳴器腳位
  pinMode(ALARM,OUTPUT);

  //車子前進
  forward();
}

void loop() {
  // put your main code here, to run repeatedly:

  // 避障功能
  // AvoidObstacle();

  // 警報功能
  ReceiveAlarm();

  // 循跡功能
  // 0=white,有反射;1=black,無反射
  Left_val = digitalRead(LeftInfrared);
  // Serial.print("infraRed-LEFT:   ");
  // Serial.println(Left_val);
  Right_val = digitalRead(RightInfrared);
  // Serial.print("infraRed-RIGHT:   ");
  // Serial.println(Right_val);

  // 左邊在偵測黑線，右邊白線 -> 往左走
  if (Left_val==1 && Right_val==0){          
    // delay(20);
    turnLeft();
  }// 左邊在偵測白線，右邊黑線 -> 往右走
  else if(Left_val==0 && Right_val==1){
    // delay(20);
    turnRight();
  }//都遇到黑線 -> 先走判斷再停止
  else if(Left_val==1 && Right_val==1){
    delay(400);
    Right_val = digitalRead(RightInfrared);
    Left_val = digitalRead(LeftInfrared);
    if(Left_val==1 && Right_val==1){
      stopMoving(); 
    }
  }else{
    forward();
  }
  
  
  
  // Serial.print;n("Distance in CM: ");
  // Serial.println(distance);
  delay(30); //每次間格0.03秒
}
