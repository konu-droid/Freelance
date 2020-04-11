#include <SPI.h>
//#include <nRF24L01.h>
//#include <RF24.h>
#include <Servo.h>

//RF24 radio(7, 8); // CE, CSN

Servo servo1,servo2; 

const byte address[6] = "00001";
int y = 0, p = 0;
int servo1_val = 45, servo2_val = 45;

void setup() {
  Serial.begin(9600);
  servo1.attach(6);
  servo2.attach(5);
  
  //radio.begin();
  //radio.openReadingPipe(0, address);
  //radio.setPALevel(RF24_PA_MIN);
  //radio.startListening();
}
void loop() {
  if (Serial.available()) {

    int text[2];

    Serial.read(&text, sizeof(text));

    servo1_val = map(text[0],0,120,0,180);
    servo1.write(servo1_val);

    servo2_val = map(text[1],0,120,0,180);
    servo2.write(servo2_val);

    Serial.print(text[0]);
    Serial.print("\t");
    Serial.println(text[1]);
  }
}
