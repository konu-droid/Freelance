#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

File logger;

/*
  SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10 (for MKRZero SD: SDCARD_SS_PIN)
*/

// connect CS pin of sd card module to pin 53 on mega
int pinCS = 10;

//connect to GND
#define up 2
#define enter 3
#define down 4
#define ok 5                    //max kn shown
#define show_date 6
#define set_kn_out 7
#define tare 8                  // zero all kn and bar

// A0,A1,A2,A3,A6,A7 are sensor pins
// A4,A5 SDA and SCL for I2C

float old_set = 0;
float set_val = 0;
float pressure_kn = 0, prev_kn = 0, set_kn = 0;
float gain = 0; //change this to adjust gain
float tmr = 0; //tare function

//edit this to increase datalogging speed in sd card
int wait_time = 5;

int wait_count = 0;

void setup() {
  byte four = EEPROM.read(201);
  byte three = EEPROM.read(202);
  byte two = EEPROM.read(203);
  byte one = EEPROM.read(204);
  unsigned int saving = ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
  set_kn = saving / 100;

  Serial.begin(9600);
  rtc.begin(); // Initialize the rtc object
  lcd.begin(20, 4);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  lcd.init();
  pinMode(ok, INPUT_PULLUP);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(tare, INPUT_PULLUP);
  pinMode(enter, INPUT_PULLUP);
  pinMode(show_date, INPUT_PULLUP);
  pinMode(set_kn_out, OUTPUT);
  pinMode(pinCS, OUTPUT);

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Made by GAGAN");
  delay(2000);

  lcd.setCursor(0, 0);
  lcd.print("   Pressure ");
  lcd.setCursor(0, 1);
  lcd.print("     Gauge  ");
  delay(1000);

  DateTime now = rtc.now();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current Time:");
  lcd.setCursor(0, 1);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current Date:");
  lcd.setCursor(0, 1);
  lcd.print(now.year(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.print(" (");
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(") ");
  delay(2000);

  logger = SD.open("log.txt", FILE_WRITE);
  if (logger) {
    logger.println("Date,Time,Bar Value,Set Bar,Maximum");
    logger.close(); // close the file
  }
  else {
    Serial.println("error opening log.txt");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BARS");
}

void(* resetFunc) (void) = 0;

void loop() {
  DateTime now = rtc.now();
  prev_kn = pressure_kn;
  int sensorVal = analogRead(A0);
  delay(100);

  pressure_kn = map(sensorVal, 0, 1024, 0, 1000);

  sensorVal = analogRead(A1);
  float pressure_kn2 = map(sensorVal, 0, 1024, 0, 1000);
  pressure_kn += pressure_kn2;

  sensorVal = analogRead(A2);
  float pressure_kn3 = map(sensorVal, 0, 1024, 0, 1000);
  pressure_kn += pressure_kn3;

  sensorVal = analogRead(A3);
  float pressure_kn4 = map(sensorVal, 0, 1024, 0, 1000);
  pressure_kn += pressure_kn4;

  sensorVal = analogRead(A6);
  float pressure_kn5 = map(sensorVal, 0, 1024, 0, 1000);
  pressure_kn += pressure_kn5;

  sensorVal = analogRead(A7);
  float pressure_kn6 = map(sensorVal, 0, 1024, 0, 1000);
  pressure_kn += pressure_kn6;

  pressure_kn = pressure_kn / 100;

  pressure_kn = (pressure_kn + gain) - tmr;

  if (digitalRead(show_date) == LOW) {
    delay(70);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Current Time:");
    lcd.setCursor(0, 1);
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    delay(2000);
    lcd.setCursor(0, 0);
    lcd.print("Current Date:");
    lcd.setCursor(0, 1);
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.print(" (");
    lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    lcd.print(") ");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BARS");
  }

  if (digitalRead(tare) == LOW) {
    /* tmr = pressure_kn;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tare...");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("BARS");*/
    resetFunc();
  }

  if (digitalRead(enter) == LOW) {
    lcd.clear();
    lcd.print("Set BARS");
    delay(300);
    old_set = 0;
    while (digitalRead(enter) != LOW) {
      old_set = set_kn;
      if (set_kn > 60 || set_kn < 0) {
        set_kn = 0;
      }
      if (digitalRead(up) == LOW && set_kn < 60) {
        delay(50);
        set_kn = set_kn + 0.01;
      }
      if (digitalRead(down) == LOW && set_kn > 0) {
        delay(50);
        set_kn = set_kn - 0.01;;
      }
      clearing(old_set, set_kn);
      lcd.setCursor(0, 1);
      lcd.print(set_kn, 2);
    }
    //add to eeprom;
    unsigned int save = set_kn * 100;
    byte four = ((save >> 0) & 0xFF);
    byte three = ((save >> 8) & 0xFF);
    byte two = ((save >> 16) & 0xFF);
    byte one = ((save >> 24) & 0xFF);
    EEPROM.write(201, four);
    EEPROM.write(202, three);
    EEPROM.write(203, two);
    EEPROM.write(204, one);
    delay(100);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BARS");
  }


  if (set_val < pressure_kn) {
    set_val = pressure_kn;
  }

  if (digitalRead(ok) == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MAX BARS");
    lcd.setCursor(0, 1);
    lcd.print(set_val);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BARS");
  }

  lcd_clear(pressure_kn, prev_kn);
  lcd.setCursor(0, 1);
  lcd.print(pressure_kn, 2);

  if (pressure_kn >= set_kn) {
    digitalWrite(set_kn_out, HIGH);
  }
  else {
    digitalWrite(set_kn_out, LOW);
  }

  if (wait_time == wait_count) {
    logger = SD.open("log.txt", FILE_WRITE);
    if (logger) {
      logger.print(now.year(), DEC);
      logger.print('/');
      logger.print(now.month(), DEC);
      logger.print('/');
      logger.print(now.day(), DEC);
      logger.print(",");
      logger.print(now.hour(), DEC);
      logger.print(':');
      logger.print(now.minute(), DEC);
      logger.print(':');
      logger.print(now.second(), DEC);
      logger.print(",");
      logger.print(pressure_kn);
      logger.print(",");
      logger.print(set_kn);
      logger.print(",");
      logger.println(set_val);
      logger.close(); // close the file
    }

    // if the file didn't open, print an error:
    else {
      Serial.println("error opening log.txt");
    }
    wait_count = 0;
  }
  else if (wait_count < wait_time) {
    wait_count++;
  }

}

void lcd_clear(float f, float prev_f) {
  f = f * 100;
  prev_f = prev_f * 100;
  if ((f < 10 && prev_f > 10) || (f < 100 && prev_f > 100) || (f < 1000 && prev_f > 1000)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BARS");
  }
}

void clearing(float old, float not_old) {
  old = old * 100;
  not_old = not_old * 100;
  if ((old >= 10 && not_old < 10) || (old >= 100 && not_old < 100) || (old >= 1000 && not_old < 1000)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set BARS");
  }
}
