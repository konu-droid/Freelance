//connect tx and rx of thermal printer to arduino D0 and D1

#include <Wire.h>
#include <EEPROM.h>
#include "RTClib.h"
#include <LiquidCrystal.h>
#include <Adafruit_Thermal.h>

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Adafruit_Thermal printer(&Serial);

#define up 6
#define down 5
#define ok 4
#define output_a A1
#define output_b A2
#define save 2
#define print_button 3

volatile double temp;
int set_temp = 0, tempe;

int t_hour, t_min, t_sec;
int target_hour, target_min, target_sec;
int remain_hour, remain_min, remain_sec;
volatile int start_hour, start_min, start_sec;
volatile int address = 9, count_address = 0;

void setup() {

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Made by GAGAN");
  delay(2000);

  DateTime now = rtc.now();

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("DATE : ");
  lcd.print(now.year(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.setCursor(0, 1);
  lcd.print("TIME : ");
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  delay(2000);

  pinMode(A0, INPUT);
  pinMode(down, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(ok, INPUT_PULLUP);
  pinMode(save, INPUT_PULLUP);
  pinMode(print_button, INPUT_PULLUP);
  pinMode(output_a, OUTPUT);
  analogWrite(output_a, LOW);
  pinMode(output_b, OUTPUT);

  byte two = EEPROM.read(201);
  byte one = EEPROM.read(202);
  int saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
  t_hour = saving;
  two = EEPROM.read(203);
  one = EEPROM.read(204);
  saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
  t_min = saving;
  two = EEPROM.read(205);
  one = EEPROM.read(206);
  saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
  t_sec = saving;
  two = EEPROM.read(207);
  one = EEPROM.read(208);
  saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
  set_temp = saving;
  attachInterrupt(0, save_para, FALLING);
  attachInterrupt(1, print_it, FALLING);
  lcd.clear();

  Serial.begin(19200);
  printer.begin();
}

void loop() {
  DateTime now = rtc.now();

  display_temp();

  if (tempe < set_temp) {
    analogWrite(output_a, HIGH);
    analogWrite(output_b, LOW);
  }
  else if (tempe == set_temp) {
    analogWrite(output_a, LOW);
    lcd.clear();
    while (digitalRead(ok) != LOW) {
      lcd.setCursor(0, 0);
      lcd.print("System Ready!");
    }
    analogWrite(output_b, HIGH);
    DateTime now = rtc.now();
    target_hour = now.hour() + t_hour;
    target_min = now.minute() + t_min;
    if (target_min >= 60) {
      target_min -= 60;
      target_hour++;
    }
    target_sec = now.second() + t_sec;
    if (target_sec >= 60) {
      target_sec -= 60;
      target_min++;
    }
    if (target_min >= 60) {
      target_min -= 60;
      target_hour++;
    }
    start_hour = now.hour();
    start_min = now.minute();
    start_sec = now.second();
    cal_remain();
    while (remain_hour == remain_min == remain_sec == 0) {
      cal_remain();
      display_temp();
      lcd.setCursor(0, 1);
      lcd.print("Remain");
      lcd.setCursor(8, 1);
      lcd.print(t_hour);
      lcd.print(":");
      lcd.print(t_min);
      lcd.print(":");
      lcd.print(t_sec);
    }
    lcd.clear();
    analogWrite(output_b, LOW);
  }

  while (digitalRead(ok) == LOW) {
    int count;
    count++;
    delay(10);
    if (count > 500) {
      lcd.clear();
      delay(250);
      while (digitalRead(ok) != LOW) {
        if (digitalRead(up) == LOW && t_hour < 99) {
          ++t_hour;
          delay(15);
        }
        if (digitalRead(down) == LOW && t_hour > 0) {
          --t_hour;
          delay(15);
        }
        display_timmer();
      }
      delay(250);
      while (digitalRead(ok) != LOW) {
        if (digitalRead(up) == LOW && t_min < 60) {
          ++t_min;
          delay(15);
        }
        if (digitalRead(down) == LOW && t_min > 0) {
          --t_min;
          delay(15);
        }
        display_timmer();
      }
      delay(250);
      while (digitalRead(ok) != LOW) {
        if (digitalRead(up) == LOW && t_sec < 60) {
          ++t_sec;
          delay(15);
        }
        if (digitalRead(down) == LOW && t_sec > 0) {
          --t_sec;
          delay(15);
        }
        display_timmer();
      }
      byte two = (t_hour & 0xFF);
      byte one = ((t_hour >> 8) & 0xFF);
      EEPROM.update(0, two);
      EEPROM.update(1, one);
      two = (t_min & 0xFF);
      one = ((t_min >> 8) & 0xFF);
      EEPROM.update(2, two);
      EEPROM.update(3, one);
      two = (t_sec & 0xFF);
      one = ((t_sec >> 8) & 0xFF);
      EEPROM.update(4, two);
      EEPROM.update(5, one);
      lcd.clear();
      lcd.print("Saved");
      delay(1000);
      lcd.clear();
      while (digitalRead(ok) != LOW) {
        if (digitalRead(up) == LOW && set_temp < 1000) {
          ++set_temp;
          delay(15);
        }
        if (digitalRead(down) == LOW && set_temp > 0) {
          --set_temp;
          delay(15);
        }
        display_temp_set();
      }
      two = (set_temp & 0xFF);
      one = ((set_temp >> 8) & 0xFF);
      EEPROM.update(6, two);
      EEPROM.update(7, one);
      lcd.clear();
      lcd.print("Saved");
      delay(1000);
    }
  }
}

void display_timmer() {
  lcd.setCursor(0, 0);
  lcd.print("Set Timmer Time");
  lcd.setCursor(0, 1);
  lcd.print(t_hour);
  lcd.print(":");
  lcd.print(t_min);
  lcd.print(":");
  lcd.print(t_sec);
}

void display_temp() {
  temp = analogRead(A0);
  temp = temp * 0.4882813;
  tempe = temp * 10;

  lcd.setCursor(0, 0);
  lcd.print("Temp       ");
  lcd.print(temp, 2);
  lcd.setCursor(15, 0);
  lcd.print("C");
}

void cal_remain() {
  DateTime now = rtc.now();
  remain_hour = target_hour - now.hour();
  remain_min  = target_min - now.minute();
  remain_sec = target_sec - now.second();
}

void display_temp_set() {
  lcd.setCursor(0, 0);
  lcd.print("Set Temp Limit");
  lcd.setCursor(0, 1);
  temp = set_temp / 10;
  lcd.print(temp);
  lcd.setCursor(15, 1);
  lcd.print("C");
}

int auto_clr(int prev, int present) {
  int c1 = 0, c2 = 0;
  while (prev / 10 != 0) {
    c1++;
  }
  while (present / 10 != 0) {
    c2++;
  }
  if (c1 > c2) {
    lcd.clear();
  }
}

void save_para() {
  DateTime now = rtc.now();

  count_address++;

  int n_hour, n_min, n_sec, save_temp;
  save_temp = temp * 10;
  byte two = (save_temp & 0xFF);
  byte one = ((save_temp >> 8) & 0xFF);
  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);

  n_hour = now.hour() - start_hour;
  n_min = now.minute() - start_min;
  n_sec = now.second() - start_sec;

  two = (n_hour & 0xFF);
  one = ((n_hour >> 8) & 0xFF);
  EEPROM.update(address + 2, two);
  EEPROM.update(address + 3, one);

  two = (n_min & 0xFF);
  one = ((n_min >> 8) & 0xFF);
  EEPROM.update(address + 4, two);
  EEPROM.update(address + 5, one);

  two = (n_sec & 0xFF);
  one = ((n_sec >> 8) & 0xFF);
  EEPROM.update(address + 6, two);
  EEPROM.update(address + 7, one);

  address += 8;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Saved");
  delay(500);
  lcd.clear();
}

void print_it() {
  printer.wake();
  int n_hour, n_min, n_sec, save_temp;

  for (int i = 0; i < count_address; i++,address+=8) {
    byte two = EEPROM.read(address);
    byte one = EEPROM.read(address + 1);
    int saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
    save_temp = saving;
    temp = save_temp / 10;
    two = EEPROM.read(address + 2);
    one = EEPROM.read(address + 3);
    saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
    n_hour = saving;
    two = EEPROM.read(address + 4);
    one = EEPROM.read(address + 5);
    saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
    n_min = saving;
    two = EEPROM.read(address + 6);
    one = EEPROM.read(address + 7);
    saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
    n_sec = saving;

    printer.justify('R');
    printer.boldOn();
    printer.print(i);
    printer.print(F(".Actual Temp: "));
    printer.boldOff();
    printer.println(temp, 2);
    printer.boldOn();
    printer.print(F("Since Time: "));
    printer.boldOff();
    printer.print(n_hour);
    printer.print(F(":"));
    printer.print(n_min);
    printer.print(F(":"));
    printer.print(n_sec);
    printer.println(F(":"));
  }

  printer.sleep();
}
