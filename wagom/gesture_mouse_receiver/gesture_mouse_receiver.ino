#include <Mouse.h>

#define on 9
int data = 0;
void setup() {
  pinMode(on, INPUT_PULLUP);
  Serial1.begin(38400);
  Serial.begin(38400);
  //  delay(5000);
  //  while(!Serial1.available()){
  //    Serial1.write(9);
  //    }
}
void loop() {
  while (digitalRead(on) == LOW) {
    if (Serial1.available()) {
      data = Serial1.read();
      //Serial.println(data);
    }
    if (data == 255 || data == 254 || data == 253 || data == 252 || data == 251 || data == 250 || data == 249 || data == 248) {
Serial.println(data);
    }
      if (data == 255) {
      Mouse.press(MOUSE_RIGHT);
      delay(200);
      Mouse.release(MOUSE_RIGHT);
      }
      else if (data == 254) {
      Mouse.press(MOUSE_LEFT);
      delay(200);
      Mouse.release(MOUSE_LEFT);
      }
      else if (data == 252) {
      Mouse.move(0, 0, 1);
      delay(50);
      }
      else if (data == 253) {
      Mouse.move(0, 0, -1);
      delay(50);
      }
      else if (data == 250) {
      Mouse.move(5, 0, 0);
      delay(7);
      }
      else if (data == 251) {
      Mouse.move(-5, 0, 0);
      delay(7);
      }
      else if (data == 248) {
      Mouse.move(0, 5, 0);
      delay(7);
      }
      else if (data == 249) {
      Mouse.move(0, -5, 0);
      delay(7);
      }
    data = 0;
  }
}
