#include <Wire.h>
#include <EEPROM.h>
#include <Stepper.h>
#include <LiquidCrystal.h>

const int rs = 22, en = 24, d4 = 26, d5 = 28, d6 = 30, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//connect to GND
#define up 2
#define down 3
#define ok 4
#define check_kn 5
#define output 10
#define forw 25
#define back 27

int count = 0;

//adjust this to change the amount of movement/ speed
const int stepsPerRevolution = 10;

//stepper pins
Stepper myStepper(stepsPerRevolution, 6, 7, 8, 9);

int set_val = 0, set_speed = 0, set_speed1 = 0;
int prev_b, prev_k;
int pressure_bar, pressure_KN;
int sensorVal = 0;
float set1 = 0, max_val = 0, pre_k, pre_b;

void setup() {
  Serial.begin(9600);
  byte two = EEPROM.read(201);
  byte one = EEPROM.read(202);

  int saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
  set_val = saving;

  two = EEPROM.read(205);
  one = EEPROM.read(206);

  saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
  set_speed1 = saving;
  set_speed = set_speed1 / 60;

  //======stepper motor speed====
  myStepper.setSpeed(60);

  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(ok, INPUT_PULLUP);
  pinMode(check_kn, INPUT_PULLUP);
  pinMode(output, OUTPUT);
  pinMode(forw, OUTPUT);
  pinMode(back, OUTPUT);
  pinMode(54, INPUT);
  pinMode(55, INPUT);
  pinMode(56, INPUT);
  pinMode(57, INPUT);
  pinMode(58, INPUT);
  pinMode(59, INPUT);
  pinMode(60, INPUT);
  pinMode(61, INPUT);
  pinMode(62, INPUT);
  pinMode(63, INPUT);
  pinMode(64, INPUT);
  pinMode(65, INPUT);
  pinMode(66, INPUT);
  pinMode(67, INPUT);
  pinMode(68, INPUT);
  pinMode(69, INPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Made by GAGAN");
  delay(2000);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("   Pressure ");
  lcd.setCursor(0, 1);
  lcd.print("     Gauge  ");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BARS");
  lcd.print("    KN");
}

void loop() {
  prev_b = pressure_bar;
  prev_k = pressure_KN;
  pressure_KN = 0;
  for (int i = 54; i < 70 ; i++) {
    sensorVal = analogRead(i);
    sensorVal = map(sensorVal, 0, 512, 0, 1000);
    pressure_KN += sensorVal;
    //  delay(15);
  }

  if (pressure_KN < 0 ) {
    pressure_KN = 0;
  }

  pressure_bar = map(pressure_KN, 0, 16000, 0, 4000);

  if (digitalRead(ok) == LOW) {
    lcd.clear();
    delay(500);
    set1 = 0;
    while (digitalRead(ok) != LOW) {
      if (digitalRead(up) == LOW && set_val < 16000) {
        delay(50);
        set_val += 1;
        lcd.clear();
      }
      else if (digitalRead(down) == LOW && set_val > 0) {
        delay(50);
        set_val -= 1;
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print("SET LIMIT");
      lcd.setCursor(0, 1);
      set1 = set_val;
      set1 = set1 / 100;
      lcd.print(set1, 2);
    }

    lcd.clear();
    delay(500);
    set1 = 0;
    while (digitalRead(ok) != LOW) {
      if (digitalRead(up) == LOW && set_speed1 < 16000) {
        delay(50);
        set_speed1 += 1;
        lcd.clear();
      }
      else if (digitalRead(down) == LOW && set_speed1 > 0) {
        delay(50);
        set_speed1 -= 1;
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print("SET SPEED");
      lcd.setCursor(0, 1);
      set1 = set_speed1;
      set1 = set1 / 100;
      lcd.print(set1, 2);
    }
    //add to eeprom;
    int save = set_val;
    byte two = (save & 0xFF);
    byte one = ((save >> 8) & 0xFF);
    EEPROM.write(201, two);
    EEPROM.write(202, one);

    save = set_speed1;
    two = (save & 0xFF);
    one = ((save >> 8) & 0xFF);
    EEPROM.write(205, two);
    EEPROM.write(206, one);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BARS");
    lcd.print("    KN");
    //   count = 0;
    set_speed = set_speed1 / 60;
  }

  if (max_val < pressure_KN) {
    max_val = pressure_KN;
    max_val = max_val / 100;
  }

  if (digitalRead(check_kn) == LOW ) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MAX KN");
    lcd.setCursor(0, 1);
    lcd.print(max_val, 2);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BARS");
    lcd.print("    KN");
  }

  pre_b = pressure_bar;
  pre_k = pressure_KN;

  pre_b = pre_b / 100;
  pre_k = pre_k / 100;
  //  Serial.print(set1 / 60);
  //  Serial.print("\t");
  //  Serial.println(set_speed);

  if (pressure_KN >= set_val) {
    digitalWrite(output, HIGH);
  }
  else if (pressure_KN < set_val) {
    digitalWrite(output, LOW);
  }

  lcd_clear(pressure_bar, prev_b);
  lcd_clear(pressure_KN, prev_k);

  lcd.setCursor(0, 1);
  lcd.print(pre_b, 2);
  lcd.print("    ");
  lcd.print(pre_k, 2);

  if ((pressure_KN - prev_k) == set_speed || set_speed == 0)
  {
    myStepper.step(0);
    digitalWrite(forw, LOW);
    digitalWrite(back, LOW);
    delay(10);
  }
  else if ((pressure_KN - prev_k) < set_speed)
  {
    open_valve();
    delay(10);
  }
  else if ((pressure_KN - prev_k) > set_speed) {
    close_valve();
    delay(10);
  }
  delay(900);

  //  count++;
  //  if (count > 3600) {
  //    count = 0;
  //  }
}

void open_valve() {
  digitalWrite(back, LOW);
  digitalWrite(forw, HIGH);
  myStepper.step(stepsPerRevolution);
  digitalWrite(back, LOW);
}

void close_valve() {
   digitalWrite(forw, LOW);
  digitalWrite(back, HIGH);
  myStepper.step(-stepsPerRevolution);
  digitalWrite(forw, LOW);
}

void lcd_clear(int f, int prev_f) {
  if ((f < 10 && prev_f >= 10) || (f < 100 && prev_f >= 100) || (f < 1000 && prev_f >= 1000) || (f < 10000 && prev_f >= 10000)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BARS");
    lcd.print("    KN");
  }
}
