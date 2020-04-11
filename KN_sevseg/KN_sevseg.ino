#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

#include <SevSeg.h>

SevSeg sevseg;

//connect to GND
#define up A1
#define down A2
#define ok A3         //max kn shown
#define set_kn_out 9
#define enter 8
#define tare A7                  // zero all kn and bar

int old_set = 0;
int set_val = 0;
int pressure_kn = 0, prev_kn = 0, set_kn = 0;
int gain = 0; //change this to adjust gain
int tmr = 0; //tare function
int sensorVal = 0;

void setup() {
  byte two = EEPROM.read(201);
  byte one = EEPROM.read(202);
  int saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
  set_kn = saving;

  byte numDigits = 4;
  byte digitPins[] = {7, 0, 1, 2};
  byte segmentPins[] = {3, 4, 5, 6, 10, 11, 12, 13};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = P_TRANSISTORS; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);

  lcd.init();
  pinMode(ok, INPUT_PULLUP);
  pinMode(A0, INPUT);
  pinMode(A6, INPUT);
  pinMode(tare, INPUT_PULLUP);
  pinMode(enter, INPUT_PULLUP);
  pinMode(set_kn_out, OUTPUT);

  lcd.backlight();
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
  lcd.print("Kilo Newton");
}

void loop() {
  if (analogRead(tare) < 1) {
    delay(150);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TARE....");
    tmr = pressure_kn;
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kilo Newton");
  }

  prev_kn = pressure_kn;
  sensorVal = analogRead(A0);

  pressure_kn = map(sensorVal, 0, 1024, 0, 1000);

  sensorVal = analogRead(A6);
  pressure_kn = pressure_kn + map(sensorVal, 0, 1024, 0, 1000);
  pressure_kn = (pressure_kn + gain) - tmr;

  if (digitalRead(enter) == LOW) {
    lcd.clear();
    lcd.print("Set KN");
    delay(300);
    old_set = 0;
    while (digitalRead(enter) != LOW) {
      old_set = set_kn;
      if (set_kn > 2000 || set_kn < 0) {
        set_kn = 0;
      }
      if (digitalRead(up) == LOW && set_kn < 2000) {
        delay(50);
        set_kn = set_kn + 1;
      }
      if (digitalRead(down) == LOW && set_kn > 0) {
        delay(50);
        set_kn = set_kn - 1;;
      }
      clearing(old_set, set_kn);
      lcd.setCursor(0, 1);
      lcd.print(set_kn);
    }
    //add to eeprom;
    int save = set_kn;
    byte two = (save & 0xFF);
    byte one = ((save >> 8) & 0xFF);
    EEPROM.write(201, two);
    EEPROM.write(202, one);
    delay(100);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kilo Newton");
  }

  if (pressure_kn >= set_kn) {
    digitalWrite(set_kn_out, LOW);
  }
  else {
    digitalWrite(set_kn_out, HIGH);
  }

  if (analogRead(ok) < 100) {
    delay(700);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MAX KN");
    lcd.setCursor(0, 1);
    lcd.print(set_val);
    for (int v = 0; v < 3000; v++) {
      sevseg.setNumber(set_val);
      sevseg.refreshDisplay();
      delay(1);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kilo Newton");
  }

  if (set_val < pressure_kn) {
    set_val = pressure_kn;
  }

  lcd_clear(pressure_kn, prev_kn);
  lcd.setCursor(0, 1);
  lcd.print(pressure_kn);

  for (int v = 0; v < 1000; v++) {
    sevseg.setNumber(pressure_kn);
    sevseg.refreshDisplay();
    delay(1);
  }
}

void lcd_clear(int f, int prev_f) {
  if ((f < 10 && prev_f >= 10) || (f < 100 && prev_f >= 100) || (f < 1000 && prev_f >= 1000)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kilo Newton");
  }
}

void clearing(int old, int not_old) {
  if ((old >= 10 && not_old < 10) || (old >= 100 && not_old < 100) || (old >= 1000 && not_old < 1000)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set KN");
  }
}
