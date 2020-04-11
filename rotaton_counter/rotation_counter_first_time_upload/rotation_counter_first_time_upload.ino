#include <LiquidCrystal.h>
#include <EEPROM.h>

#define sensor 13
// when ever the sensor goes high the program counts
// sensor is the pin where hall effect sensor is to be connected
// start_but is the pin from start button connect the other terminal of the start button to ground
// inr is the pin for increasing button connect second terminal to ground
// dcr is the pin for decreasing button connect second terminal to ground

int count = 0, start_but = 7, inr = 8, dcr = 9, output = 10;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(38400);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COUNTING MACHINE");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("original sketch");
  lcd.setCursor(0, 1);
  lcd.print("ver.1.0  GAGAN ");
  delay(3000);

  count=200;
  
  byte two = (count & 0xFF);
  byte one = ((count >> 8) & 0xFF);
  EEPROM.write(201, two);
  EEPROM.write(202, one);
  
  two = EEPROM.read(201);
  one = EEPROM.read(202);
  count = ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);

  pinMode(start_but, INPUT_PULLUP);
  pinMode(inr, INPUT_PULLUP);
  pinMode(dcr, INPUT_PULLUP);
  pinMode(sensor, INPUT);
  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);
  lcd.clear();
}

void loop() {

  int i = 0;
  lcd.setCursor(0, 0);
  lcd.print("count");
  lcd.setCursor(0, 1);
  lcd.print(count);
  if (digitalRead(inr) == LOW && count < 1050) {
    delay(50);
    count++;
    lcd.clear();
    lcd.setCursor(0, 0);
  lcd.print("count");
  lcd.setCursor(0, 1);
  lcd.print(count);
  }

  if (digitalRead(dcr) == LOW && count > 0) {
    delay(50);
    count--;
    lcd.clear();
    lcd.setCursor(0, 0);
  lcd.print("count");
  lcd.setCursor(0, 1);
  lcd.print(count);
  }

  if (digitalRead(start_but) == LOW) {
    delay(50);
    byte two = (count & 0xFF);
    byte one = ((count >> 8) & 0xFF);
    EEPROM.write(201, two);
    EEPROM.write(202, one);
    lcd.clear();
    while (count > i) {
      lcd.setCursor(0, 0);
      lcd.print("COUNTING...");
      digitalWrite(output, HIGH);
      if (digitalRead(sensor) == HIGH) {
        i++;
      }
    }
    digitalWrite(output, LOW);
  }
}
