#include <Servo.h>

Servo servo[4][3];  // create servo object to control a servo
int val[4][3];
int legNo;
int i, j;
int k = 0, m = 0, n = 0;
int sw1 = 0;
int sw2 = 0;

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  //0 is base //1 is mid //2 is leg
  servo[0][0].attach(7);   //LF
  servo[0][1].attach(8);
  servo[0][2].attach(9);
  servo[1][0].attach(4);   //RF
  servo[1][1].attach(5);
  servo[1][2].attach(6);
  servo[2][0].attach(A3);  //LB
  servo[2][1].attach(A2);
  servo[2][2].attach(A1);
  servo[3][0].attach(10);   //RB
  servo[3][1].attach(11);
  servo[3][2].attach(A0);


  inti();
  delay(7000);
      for(int a=0;a<4;a++){
    forward();
    }

}

void loop() {
  sw1 = digitalRead(2);
  sw2 = digitalRead(3);
  if (sw1 == LOW) {
    inti();
    delay(30000);
  }
  if (sw2 == LOW) {
    for(int a=0;a<4;a++){
    forward();
    }
    }
}


