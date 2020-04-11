#include <EEPROM.h>
#include <HX711.h>
#include <LiquidCrystal.h>
#include <Wire.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int s_zero = A4 , up = A2, down = A3, mode = 6, Select = 7 ;

long max_show, tmr, ten_grams;
const long max_val = 16777215;
float act_v, set_v, fake_measure, mini = 0, maxi = 50, avr;

bool unit, flag_fake = 0, flag_limit = 0;

HX711 scale(A1, A0);

void setup() {
  Serial.begin(38400);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WEIGHT MACHINE");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("original sketch");
  lcd.setCursor(0, 1);
  lcd.print("ver.1.0  KONU ");
  delay(3000);

  byte four = EEPROM.read(201);
  byte three = EEPROM.read(202);
  byte two = EEPROM.read(203);
  byte one = EEPROM.read(204);
  ten_grams = ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);

  byte four1 = EEPROM.read(205);
  byte three1 = EEPROM.read(206);
  byte two1 = EEPROM.read(207);
  byte one1 = EEPROM.read(208);
  tmr = ((four1 << 0) & 0xFF) + ((three1 << 8) & 0xFFFF) + ((two1 << 16) & 0xFFFFFF) + ((one1 << 24) & 0xFFFFFFFF);

  avr = 0;
  pinMode(s_zero, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(mode, INPUT_PULLUP);
  pinMode(Select, INPUT_PULLUP);
  // calibration();
  lcd.clear();
}

void loop() {
  top_line();
  if (digitalRead(mode)==LOW){
    delay(250);
    lcd.clear();
    menu_dis();
    delay(250);
    }
    if(digitalRead(s_zero)==LOW){
      delay(250);
      lcd.clear();
      tmr_set();
      }
}
