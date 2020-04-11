#include <HX711.h>
#include <LiquidCrystal.h>
#include <Wire.h>

//connect tmr button to pin 4 and the other terminal to gnd
#define tmr 4

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

HX711 scale(A1, A0);
float old_w = 0,weight2 = 0;
long int zero = 0,weight = 0;
// edit this to change value
float calib = 0;

void setup() {
  Serial.begin(38400);
  lcd.begin(16, 2);
  pinMode(tmr,INPUT_PULLUP);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("original sketch");
  lcd.setCursor(0, 1);
  lcd.print("ver.1.0  GAGAN");
  delay(3000);

}

void loop() {
  long int raw = scale.read();
  if (digitalRead(tmr) == LOW) {
    delay(200);
    zero = scale.get_value(5);
    lcd.clear();
  }
  weight = map(raw, zero, 16777215, 0, 2086500);
  weight2 = float(weight);
  weight2 = weight / 100;
  float new_w = weight2 + calib;
  lcd.setCursor(0, 0);
  lcd.print("WEIGHT MACHINE");
  lcd.setCursor(0, 1);
  lcd.print(new_w, 2);
  lcd.setCursor(14, 1);
  lcd.print("KG");

  if ((old_w > 10000 && 10000 > new_w) || (old_w > 1000 && 1000 > new_w) || (old_w > 100 && 100 > new_w) || (old_w > 10 && 10 > new_w)) {
    lcd.clear();
  }
  old_w = new_w;
}
