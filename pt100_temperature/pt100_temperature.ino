#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <LiquidCrystal.h>

Adafruit_ADS1015 ads;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

float temp = 0;
int16_t adc0, old_adc;


void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Made by GAGAN");
  delay(2000);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("   Temperature ");
  lcd.setCursor(0, 1);
  lcd.print("     Sensor  ");
  delay(1000);
  lcd.clear();

  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  ads.begin();
}

void loop() {
  old_adc = adc0;

  adc0 = ads.readADC_SingleEnded(0);
  adc0 = map(adc0, 26656, 32704, 0, 10000);

  temp = adc0;
  temp = adc0 / 100;

  clear_lcd(old_adc, adc0);

  lcd.setCursor(0, 0);
  lcd.print("Temperature");
  lcd.setCursor(0, 1);
  lcd.print(temp, 2);
  lcd.setCursor(15, 1);
  lcd.print("C");

}

void clear_lcd(int16_t old, int16_t neww) {
  if ((old >= 10 && neww < 10) || (old >= 100 && neww < 100) || (old >= 1000 && neww < 1000) ) {
    lcd.clear();
  }
}
