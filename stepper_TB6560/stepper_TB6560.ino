//connect A0 pin to pot for speed control
// pin 8 to cw+ on controller
// pin 9 to clk+ on controller
// no need to connect EN pin
// connect cw-,clk-,EN- to the GND
// for 1/8 step keep  S3 and S4 on and 1/16 step keep S4 only on
// connect pin 7 to a switch for direction control

int steps = 0;
int sped = 0;
#define clk 9
#define cw 8
#define direction_pin 7 

void setup() {
  pinMode(cw, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(direction_pin, INPUT_PULLUP);
  pinMode(A0, INPUT);
  digitalWrite(cw, LOW);
  digitalWrite(clk, LOW);
}

void loop() {
  if (digitalRead(direction_pin) == LOW) {
    digitalWrite(cw, LOW);
  }
  else {
    digitalWrite(cw, HIGH);
  }
  sped = analogRead(A0);
  digitalWrite(clk, HIGH);
  delayMicroseconds(sped);
  digitalWrite(clk, LOW);
  delayMicroseconds(sped);

}
