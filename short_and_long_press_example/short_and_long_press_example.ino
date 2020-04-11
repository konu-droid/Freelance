
int button = 7;


long buttonTimer = 0;
long longPressTime = 100;
long longestpress = 250;

void setup() {
  pinMode(button, INPUT_PULLUP);
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  while (digitalRead(button) == LOW) {
    buttonTimer += 1;
    Serial.println(buttonTimer);
    delay(10);
  }

  if ((buttonTimer > 1) && (buttonTimer < longPressTime) && (buttonTimer < longestpress))
  {
    Serial.println("short and a is");
    Serial.println(buttonTimer);
    buttonTimer = 0;
  }
  else if ((buttonTimer > longPressTime) && (buttonTimer < longestpress)) {

    Serial.println("long press and a is");
    Serial.println(buttonTimer);
    buttonTimer = 0;
  }
  else if (buttonTimer > longestpress) {
    Serial.println("longest press and a is");
    Serial.println(buttonTimer);
    buttonTimer = 0;
  }
  buttonTimer = 0;
}
