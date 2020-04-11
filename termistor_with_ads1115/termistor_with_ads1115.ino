//used a simple termistor along with 10k resistor and scaled it for a larger range using ads1115

#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <LiquidCrystal.h>

Adafruit_ADS1115 ads(0x48);

float Voltage = 0.0;

int thermistor_25 = 10000;

float refCurrent = 0.0001;
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

float set_temp = 0, hyst = 0, diff = 0;
int new_sped = 0;

#define ok 8
#define up 9
#define down 10

#define heater 11
#define cooler 12

int PID_heater = 20;
int PID_cooler = 20;

bool flag = 1;

void setup(void)
{
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Made by GAGAN");
  delay(2000);

  pinMode(ok, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);

  pinMode(heater, OUTPUT);

  Serial.begin(9600);

  // Descomentar el que interese
  // ads.setGain(GAIN_TWOTHIRDS);  +/- 6.144V  1 bit = 0.1875mV (default)
  // ads.setGain(GAIN_ONE);        +/- 4.096V  1 bit = 0.125mV
  // ads.setGain(GAIN_TWO);        +/- 2.048V  1 bit = 0.0625mV
  // ads.setGain(GAIN_FOUR);       +/- 1.024V  1 bit = 0.03125mV
  // ads.setGain(GAIN_EIGHT);      +/- 0.512V  1 bit = 0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    +/- 0.256V  1 bit = 0.0078125mV
  ads.begin();
  lcd.clear();
}

void loop() {

  int16_t adc0; // we read from the ADC, we have a sixteen bit integer as a result

  adc0 = ads.readADC_SingleEnded(0); // Read ADC value from ADS1115

  Voltage = adc0 * (5.0 / 65535); // Replace 5.0 with whatever the actual Vcc of your Arduino is

  float resistance = (Voltage / refCurrent); // Using Ohm's Law to calculate resistance of thermistor

  float ln = log(resistance / thermistor_25); // Log of the ratio of thermistor resistance and resistance at 25 deg. C

  float kelvin = 1 / (0.0033540170 + (0.00025617244 * ln) + (0.0000021400943 * ln * ln) + (-0.000000072405219 * ln * ln * ln)); // Using the Steinhart-Hart Thermistor Equation to calculate temp in K

  float temp = kelvin - 273.15 - 0.68; // Converting Kelvin to Celcuis -0.610 is offset set through calibration factor

  Serial.print("AIN0: "); // Print ADC value to Serial Monitor

  Serial.print(adc0);

  Serial.print("\tTemperature: "); // Print temperature to Serial Monitor in Celcius


  Serial.println(temp, 7);
  lcd.setCursor(0, 1);
  lcd.print("Temp = ");
  lcd.setCursor(7, 1);
  lcd.print(temp, 3);
  lcd.setCursor(14, 1);
  lcd.print(" C");
  Serial.println();

  if (digitalRead(ok) == LOW) {
    lcd.clear();
    delay(100);

    while (digitalRead(ok) != LOW) {
      lcd.setCursor(0, 0);
      lcd.print("Set Temp");

      if (digitalRead(up) == LOW) {
        set_temp += 0.01;
        delay(50);
      }
      else if (digitalRead(down) == LOW) {
        set_temp -= 0.01;
        delay(50);
      }
      lcd.setCursor(0, 1);
      lcd.print(set_temp, 3);
      lcd.println(" C ");
    }
    lcd.clear();
    while (digitalRead(ok) != LOW) {
      lcd.setCursor(0, 0);
      lcd.print("Set Hyst");

      if (digitalRead(up) == LOW) {
        hyst += 0.01;
        delay(50);
      }
      else if (digitalRead(down) == LOW) {
        hyst -= 0.01;
        delay(50);
      }
      lcd.setCursor(0, 1);
      lcd.print(hyst, 3);
      lcd.println(" C ");
    }
  }

  if (temp == set_temp) {
    flag = 0;
  }
  if (temp > set_temp + hyst || temp < set_temp - hyst) {
    flag = 1;
  }

  if (temp < set_temp && flag == 1) {
    diff = set_temp - temp;

    if (diff < PID_heater)
    {
      new_sped = map(diff, 0, PID_heater, 0, 100);
      switching(new_sped, heater, cooler);
    }
    else if (flag == 1) {
      digitalWrite(heater, HIGH);
      digitalWrite(cooler, LOW);
    }
  }
  else {
    diff = temp - set_temp;
    if (diff < PID_cooler)
    {
      new_sped = map(diff, 0, PID_cooler, 0, 100);
      switching(new_sped, cooler, heater);
    }
    else {
      digitalWrite(heater, LOW);
      digitalWrite(cooler, HIGH);
    }
  }

}

void switching(int sped, int pin, int pin2)
{
  digitalWrite(pin2, LOW);
  digitalWrite(pin, HIGH);
  delay(sped);
  digitalWrite(pin, LOW);
  delay(100 - sped);

}
