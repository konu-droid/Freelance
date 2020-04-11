#include <LiquidCrystal.h>
#include <EEPROM.h>

#define sensor 13
// when ever the sensor goes high the program counts
// sensor is the pin where hall effect sensor is to be connected
// start_but is the pin from start button connect the other terminal of the start button to ground
// inr is the pin for increasing button connect second terminal to ground
// dcr is the pin for decreasing button connect second terminal to ground

int  start_but = 8, inr = 6, dcr = 7, output = 10 , rst = 2;
volatile long i = 0, count = 0;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

volatile bool paused = 0;

int working_time = 5; //adjust this to change no of working hours
int rest_time = 1;  //adjust this to change the rest time of the counter

int timer = 0, j = 0, k = 0, l = 0;

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

  EEPROM.get(5,paused);

  EEPROM.get(0, count);

  EEPROM.get(6, i);
  
  pinMode(start_but, INPUT_PULLUP);
  pinMode(inr, INPUT_PULLUP);
  pinMode(dcr, INPUT_PULLUP);
  pinMode(rst, INPUT_PULLUP);
  pinMode(sensor, INPUT);
  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);
  lcd.clear();

  attachInterrupt(0, mem_reset, FALLING);
}

void loop() {

  lcd.setCursor(0, 0);
  lcd.print("count");
  lcd.setCursor(0, 1);
  lcd.print(count);
  if (digitalRead(inr) == LOW && count < 2000000) {
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

  if (digitalRead(start_but) == LOW || (paused == 1)) {
    if (paused == 0) {
      EEPROM.put(0, count);
    }
    lcd.clear();
    delay(100);
    paused = 1;
    EEPROM.put(5, paused);
    digitalWrite(output, HIGH);

    while (count > i) {
      lcd.setCursor(0, 0);
      lcd.print("COUNTING...");
      lcd.setCursor(0, 1);
      lcd.print(i);

      if (timer == working_time) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("rest");
        timer = 0;
        j = 0;
        k = 0;
        l = 0;
        digitalWrite(output, LOW);
        while (timer != rest_time) {
          j++;
          delay(10);
          if (j > 100) {
            k++;
            j = 0;
          }
          if (k == 60) {
            l++;
            k = 0; j = 0;
          }
          if (l == 60) {
            timer++;
            l = 0; k = 0; j = 0;
          }
        }
        digitalWrite(output, HIGH);
        timer = 0; l = 0; k = 0; j = 0;
      }
      else if (digitalRead(sensor) == HIGH) {
        ++i;
        EEPROM.put(6, i);
        delay(10);

        while (digitalRead(sensor) == HIGH) {
          j++;
          delay(10);
          if (j > 100) {
            k++;
            j = 0;
          }
          if (k > 60) {
            l++;
            k = 0; j = 0;
          }
          if (l > 60) {
            timer++;
            l = 0; k = 0; j = 0;
          }
        }
      }
      else {
        j++;
        delay(10);
        if (j > 100) {
          k++;
          j = 0;
        }
        if (k > 60) {
          l++;
          k = 0; j = 0;
        }
        if (l > 60) {
          timer++;
          l = 0; k = 0; j = 0;
        }
      }

    }
    mem_reset();
  }
  digitalWrite(output, LOW);
}

void mem_reset() {
  i = 0 ;
  EEPROM.put(6, i);
  paused = 0;
  EEPROM.put(5, paused);
  lcd.clear();
}
