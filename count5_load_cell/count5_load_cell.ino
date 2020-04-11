#include "HX711.h"

// HX711.DOUT  - pin #A1
// HX711.PD_SCK - pin #A0

// HX711.DOUT  - pin #A3
// HX711.PD_SCK - pin #A2

HX711 scale1(A1, A0);
HX711 scale2(A3, A2);

//const long int max_val = 16777215;
const long int gm200 = 671088;

int count = 0;

void setup() {
  Serial.begin(38400);
  scale1.set_scale(2280.f);
  scale2.set_scale(2280.f);

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale1.read());

  Serial.print("get value: \t\t");
  Serial.println(scale1.get_value(5));

  pinMode(4, OUTPUT);   //red led
  pinMode(5, OUTPUT);   //green led

  //connect the gnd of the led to gnd of arduino and positive to digtal pins
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
}

void loop() {
  Serial.print("average reading:\t");
  Serial.print(scale1.get_value(5));
  Serial.print("\t");
  Serial.println(scale2.get_value(5));

  if (scale1.get_value(5) == gm200 && count < 5) {
    count++;
    while (scale1.get_value(5) < 1000) {
      //do nothing wait for the scale to clear
    }
  }
  
  if (scale2.get_value(5) == gm200 && count > 0) {
    count--;
    while (scale1.get_value(5) < 1000) {
      //do nothing wait for the scale to clear
    }
  }

  if (count < 5) {
    digitalWrite(5, HIGH);
    digitalWrite(4, LOW);
  }
  else {
    digitalWrite(5, LOW);
    digitalWrite(4, HIGH);
  }
}
