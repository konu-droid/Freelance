#include<Servo.h>

Servo l1;
Servo l2;
Servo l3;
Servo l4;
Servo l5;
Servo h1;
Servo h2;
Servo h3;

char i;
int j = 0;

void setup() {
  Serial.begin(9600);
  l1.attach(7);
  l2.attach(6);
  l3.attach(4);
  l4.attach(3);
  l5.attach(2);
  h1.attach(13);
  h2.attach(12);
  h3.attach(11);
  align_90();
}

void loop() {
  Serial.println("select motor number");
  while (!Serial.available()) {}
  i = Serial.read();
  Serial.println(i);
  j=0;
  switch (i) 
  {
    case '1':
      Serial.print("enter value of l1:");
      while (!Serial.available() && j<549 && j>2400) {}
      j = Serial.read();
      l1.write(j);
      Serial.println(j);
      break;

    case 2:
      Serial.print("enter value of l2:");
      while (!Serial.available()&& j<549 && j>2400) {}
      j = Serial.read();
      l2.write(j);
      Serial.println(j);
      break;

    case 3:
      Serial.print("enter value of l3:");
      while (!Serial.available()&& j<549 && j>2400) {}
      j = Serial.read();
      l3.write(j);
      Serial.println(j);
      break;

    case 4:
      Serial.print("enter value of l4:");
      while (!Serial.available()&& j<549 && j>2400) {}
      j = Serial.read();
      l4.write(j);
      Serial.println(j);
      break;

    case 5:
      Serial.print("enter value of l5:");
      while (!Serial.available()&& j<549 && j>2400) {}
      j = Serial.read();
      l5.write(j);
      Serial.println(j);
      break;

    case 6:
      Serial.print("enter value of h1:");
      while (!Serial.available()&& j<549 && j>2400) {}
      j = Serial.read();
      h1.write(j);
      Serial.println(j);
      break;

    case 7:
      Serial.print("enter value of h2:");
      while (!Serial.available()&& j<549 && j>2400) {}
      j = Serial.read();
      h2.write(j);
      Serial.println(j);
      break;

    case 8:
      Serial.print("enter value of h3:");
      while (!Serial.available()&& j<549 && j>2400) {}
      j = Serial.read();
      h3.write(j);
      Serial.println(j);
      break;

    default:
      Serial.print("Invalid value");
      break;
  }
}

void align_90() {
  l1.write(90);
  l2.write(90);
  l3.write(90);
  l4.write(90);
  l5.write(90);
  h1.write(90);
  h2.write(90);
  h3.write(90);
}
