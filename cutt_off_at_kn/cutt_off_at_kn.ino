//#include <EEPROM.h>
#include <LiquidCrystal.h>
LiquidCrystal LCD(12, 11, 5, 4, 3, 2);

//connect to GND
#define up A1
#define down A2
#define ok A3         //menu
#define rst 6           //reset button
#define tare 7

float set_val = 0;
float pressure_bar = 0, prev_bar = 0 ;
float pressure_kn = 0, prev_kn = 0;
float gain = 0.0; //change this to adjust gain
float tmr = 0.0; //tare function

void setup() {
  Serial.begin(9600);
  //EEPROM.get(0, set_val);

  pinMode(ok, INPUT_PULLUP);
  pinMode(A0, INPUT);
  pinMode(rst, INPUT_PULLUP);
  pinMode(tare, INPUT_PULLUP);

  LCD.begin(20, 4);
  LCD.clear();
  LCD.setCursor(0, 1);
  LCD.print("Made by GAGAN");
  delay(2000);

  LCD.begin(16, 2);
  LCD.setCursor(0, 1);
  LCD.print("   Pressure ");
  LCD.setCursor(0, 2);
  LCD.print("     Gauge  ");
  delay(1000);


  LCD.clear();
  LCD.setCursor(0, 1);
  LCD.print("BARS");
  LCD.print("      KN");
}

void loop() {
  digitalWrite(6, LOW);
  prev_bar = pressure_bar;
  prev_kn = pressure_kn;
  int sensorVal = analogRead(A0);
  pressure_bar = map(sensorVal, 0, 1024, 0, 20000);

  if (digitalRead(tare) == LOW) {
    delay(150);
    tmr = pressure_bar;
  }

  pressure_bar = pressure_bar - tmr + (gain * 100);
  pressure_kn = map(pressure_bar, 0, 9800, 0, 50000);
  pressure_kn = pressure_kn / 100;
  pressure_bar = pressure_bar / 100;

  if (digitalRead(rst) == LOW) {
    delay(150);
    set_val = 0;
  }
  if (analogRead(ok) < 100) {
    delay(500);
    LCD.clear();
    LCD.setCursor(0, 1);
    LCD.print("MAX KN");
    LCD.setCursor(0, 2);
    LCD.print(set_val);
    delay(2000);
    LCD.clear();

  }

  if (set_val < pressure_kn) {
    set_val = pressure_kn;
  }

  lcd_clear(pressure_bar, prev_bar);
  lcd_clear(pressure_kn, prev_kn);
  LCD.setCursor(0, 1);
  LCD.print(pressure_bar, 2);
  LCD.setCursor(10, 2);
  LCD.print(pressure_kn, 2);
  Serial.print("BARS = ");
  Serial.print(pressure_bar);
  Serial.println(" bars");
  Serial.print("KN = ");
  Serial.print(pressure_kn);
  Serial.print("Pressure = ");
}

void lcd_clear(float f, float prev_f) {
  if ((f < 10 && prev_f > 10) || (f < 100 && prev_f > 100) || (f < 1000 && prev_f > 1000)) {
    LCD.clear();
    LCD.setCursor(0, 1);
    LCD.print("BARS");
    LCD.print("      KN");
  }
}
