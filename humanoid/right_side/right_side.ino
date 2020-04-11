#include<Servo.h>
#include <SoftwareSerial.h>

//SoftwareSerial robo(9, 8);
SoftwareSerial mySerial(10, 5); //RX ,TX

float ypr[3] = {0, 0, 0};

Servo l1;
Servo l2;
Servo l3;
Servo l4;
Servo l5;
Servo h1;
Servo h2;
Servo h3;


typedef struct {
  float yaw;
  float pitch;
  float roll;
} attitude;

void setup() {
  //Serial.begin(38400);
  //robo.begin(9600);
  mySerial.begin(9600);
  l1.attach(7);
  l2.attach(6);
  l3.attach(4);
  l4.attach(3);
  l5.attach(2);
  h1.attach(11);
  h2.attach(12);
  h3.attach(13);
  align_90();
  //  while (!mySerial.available()) {}
  //  if (mySerial.available()) {
  //    delay(3000);
  //  }
  //  set_zero();
}

void loop() {
  //for right side fall increase it
  /*
    attitude myatt;
    if (mySerial.available() > 0) {
    mySerial.readBytes((char *)&myatt, sizeof(attitude));
    Serial.print("y = ");
    Serial.print(myatt.yaw);
    Serial.print("  p = ");
    Serial.print(myatt.pitch);
    Serial.print("  r = ");
    Serial.println(myatt.roll);
    }

    balance_pitch(myatt.pitch);
  */
  kneebend();
  //align_90();
  while (mySerial.available()) {
    char ch = mySerial.read();
    switch (ch) {
      case 'M':
        //wave_hand();
        break;
      case 'S':
        pose();
        delay(3000);
        break;
      case 'N':

        align_90();
        delay(3000);
        
        break;
    }
  }

}

void align_90() {
  l1.write(90);
  l2.write(100);
  l3.write(95);
  l4.write(75);
  l5.write(90);
  h1.write(10);
  h2.write(170);
  h3.write(90);
}
void kneebend()
 {
 l1.write(55);
  l2.write(170);
  l3.write(10);
  l4.write(120);
  l5.write(90);
  h1.write(10);
  h2.write(170);
  h3.write(90);
//  head.write(90);
 }

void oneleg() {
  l1.write(80);
  l2.write(100);
  l3.write(95);
  l4.write(100);
  l5.write(85);
  h1.write(10);
  h2.write(170);
  h3.write(90);
}

void bend() {
  l1.write(100);
  l2.write(150);
  l3.write(70);
  l4.write(80);
  l5.write(120);
  h1.write(10);
  h2.write(170);
  h3.write(90);
}

void lean() {
  l1.write(80);
  l2.write(100);
  l3.write(95);
  l4.write(100);
  l5.write(85);
  h1.write(10);
  h2.write(170);
  h3.write(90);
}

void balance_pitch(float pit) {
  if (pit > 0.0) {
    int i = l5.read();
    i++;
    l5.write(i);
    delay(15);
  }
  else if (pit < 0.0) {
    int i = l5.read();
    i--;
    l5.write(i);
    delay(15);
  }
}

/*
  void set_zero() {
  if (mySerial.available() > 0) {
    attitude myatt;
    mySerial.readBytes((char *)&myatt, sizeof(attitude));;
    ypr[0] = myatt.yaw;
    ypr[1] = myatt.pitch;
    ypr[2] = myatt.roll;
  }
  }*/


void pose() {
  // both hands up
  l1.write(55);
  l2.write(170);
  l3.write(10);
  l4.write(120);
  l5.write(90);
  h1.write(10);
  h2.write(0);
  h3.write(20);
  
}
