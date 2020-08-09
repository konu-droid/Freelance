#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
//#include <Adafruit_ADS1015.h>
#include <HX711.h>

// HX711.DOUT  - pin #A1
// HX711.PD_SCK - pin #A0


HX711 scale(A0, A1);    // parameter "gain" is ommited; the default value 128 is used by the library

//Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//connect to GND
#define up 11
#define down 12
#define ok 2
#define check_kn 3
#define output 4
#define forw A3
#define back A2
#define mode 13

long set_val = 0, set_speed = 0, set_speed1 = 0;
long prev_k;
long pressure_KN;

float set1 = 0 , max_val = 0, max_val1 = 0, pre_k, pre_b; //max_val = 0
//change this value to slowdown the program
int slow_time = 100;


void setup() {
  Serial.begin(57600);
  Serial.println(scale.read());
  byte four = EEPROM.read(201);
  byte three = EEPROM.read(202);
  byte two = EEPROM.read(203);
  byte one = EEPROM.read(204);

  int saving = ((four << 0) & 0xFFFFFF) + ((three << 8) & 0xFFFFFFFF) + ((two << 16) & 0xFFFFFFFFFF) + ((one << 24) & 0xFFFFFFFFFFFF);
  set_val = saving;

  four = EEPROM.read(206);
  three = EEPROM.read(207);
  two = EEPROM.read(208);
  one = EEPROM.read(209);

  saving = ((four << 0) & 0xFFFFFF) + ((three << 8) & 0xFFFFFFFF) + ((two << 16) & 0xFFFFFFFFFF) + ((one << 24) & 0xFFFFFFFFFFFF);
  set_speed1 = saving;
  set_speed = set_speed1 / 60;

  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(ok, INPUT_PULLUP);
  pinMode(check_kn, INPUT_PULLUP);
  pinMode(mode, INPUT_PULLUP);
  pinMode(output, OUTPUT);
  pinMode(forw, OUTPUT);
  pinMode(back, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Made by GAGAN");
  delay(100);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("   Pressure ");
  lcd.setCursor(0, 1);
  lcd.print("     Gauge  ");
  delay(100);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("KN");

  //  ads.begin();

}

void loop() {

  // long adc0, adc1, adc2, adc3;

  if (digitalRead(mode) == HIGH) {
    prev_k = pressure_KN;
    pressure_KN = 0;
    //    adc0 = ads.readADC_SingleEnded(0);
    //  adc0 = map(adc0, 0, 26666, 0, 25000);
    //adc1 = ads.readADC_SingleEnded(1);
    //adc1 = map(adc1, 0, 26666, 0, 25000);
    //adc2 = ads.readADC_SingleEnded(2);
    //adc2 = map(adc2, 0, 26666, 0, 25000);
    //adc3 = ads.readADC_SingleEnded(3);
    //adc3 = map(adc3, 0, 26666, 0, 25000);

    pressure_KN = 0L;
    //    pressure_KN = 0L + adc0 + adc1 + adc2 + adc3; //  = 0L + adc0 + adc2 + adc3; previous values is like this bhai
  }
  else {
    prev_k = pressure_KN;
    pressure_KN = 0;

    pressure_KN = scale.get_value();

    pressure_KN = map(pressure_KN, 0, -19290, 0, 93);//  0, -19200, 0, 95)speed matcing with utm display 95 +- editing only//19999

  }


  if (pressure_KN < 0) {
    pressure_KN = -1L;
  }

  if (digitalRead(ok) == LOW) {
    lcd.clear();
    delay(500);
    set1 = 0;
    if (digitalRead(mode) == HIGH) {
      while (digitalRead(ok) != LOW) {
        if (digitalRead(up) == LOW && set_val < 100000) {
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
        if (digitalRead(up) == LOW && set_speed1 < 100000) {
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
    }
    else {
      while (digitalRead(ok) != LOW) {
        if (digitalRead(up) == LOW && set_val < 20000) {
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
        if (digitalRead(up) == LOW && set_speed1 < 20000) {
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
    }
    //add to eeprom;
    long save = set_val;
    byte four = (save & 0xFF);
    byte three = ((save >> 8) & 0xFF);
    byte two = ((save >> 16) & 0xFF);
    byte one = ((save >> 24) & 0xFF);
    EEPROM.write(201, four);
    EEPROM.write(202, three);
    EEPROM.write(203, two);
    EEPROM.write(204, one);

    save = set_speed1;
    four = (save & 0xFF);
    three = ((save >> 8) & 0xFF);
    two = ((save >> 16) & 0xFF);
    one = ((save >> 24) & 0xFF);
    EEPROM.write(206, four);
    EEPROM.write(207, three);
    EEPROM.write(208, two);
    EEPROM.write(209, one);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("KN");
    //   count = 0;
    set_speed = set_speed1 / 60;
  }



  if (max_val < pressure_KN)
  {
    max_val = pressure_KN;
    max_val1 = max_val / 100 ; //  is / max_val/ 100
  }

  if (digitalRead(check_kn) == LOW ) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("MAX KN = ");
    lcd.setCursor(10, 1);
    lcd.print(max_val1, 2 ); // actual is max_val2
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("KN");
  }


  pre_k = pressure_KN;

  pre_k = pre_k / 100;

  if (pressure_KN >= set_val) {
    digitalWrite(output, HIGH);
  }
  else if (pressure_KN < set_val) {
    digitalWrite(output, LOW);
  }
  lcd_clear(pressure_KN, prev_k);

  lcd.setCursor(8, 0);
  lcd.print(pre_k, 2);


  if ((pressure_KN - prev_k) == set_speed || set_speed == 0)
  {
    digitalWrite(forw, LOW);
    digitalWrite(back, LOW);
    delay(10);
  }
  else if ((pressure_KN - prev_k) < set_speed)
  {
    digitalWrite(back, LOW);
    digitalWrite(forw, HIGH);
    digitalWrite(back, LOW);
    delay(10);
  }
  else if ((pressure_KN - prev_k) > set_speed) {
    digitalWrite(forw, LOW);
    digitalWrite(back, HIGH);
    digitalWrite(forw, LOW);
    delay(10);
  }
  delay(slow_time);
}
void lcd_clear(int f, int prev_f) {
  if ((f < 10 && prev_f >= 10) || (f < 100 && prev_f >= 100) || (f < 1000 && prev_f >= 1000) || (f < 10000 && prev_f >= 10000) || (f < 100000 && prev_f >= 100000)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("KN");
  }
}