//PID controlled temperature control with termistor and ads1115
//with two buttoms for seting the value and it automatics saves in eeprom 

#include <PID_v1.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

Adafruit_ADS1115 ads(0x48);

float Voltage = 0.0;
int thermistor_25 = 10000;
float refCurrent = 0.001;

#define SAMPLES 5

int samples[SAMPLES];

//Define Variables to connect to
double Setpoint, Average, Output, TempProm;
int save_val;

double consKp = 1, consKi = 0.05, consKd = 0.01;

//Specify the links and initial tuning parameters
PID myPID(&TempProm, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

int PWM_Pin = 10;
int HotPin = 8;
int ColdPin = 9;

//memory storage
byte mem[4];
int address = 0;

#define up 11
#define down 12

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup()
{
  Serial.begin(9600);
  //LM35 setup

  //initialize the variables
  mem[0] = EEPROM.read(address);
  mem[1] = EEPROM.read(address + 1);
  mem[2] = EEPROM.read(address + 2);
  mem[3] = EEPROM.read(address + 3);
  save_val = (mem[0] << 24) | (mem[1] << 16) | (mem[2] << 8) | mem[3];
  Setpoint = save_val/100; // in ºC

  //turn the PID on
  myPID.SetMode(AUTOMATIC);


  //__pinmode
  pinMode(PWM_Pin, OUTPUT);
  pinMode(HotPin, OUTPUT);
  pinMode(ColdPin, OUTPUT);

  myPID.SetOutputLimits(-255, 255);

  // Initialize Pins
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Made by GAGAN");
  delay(2000);

  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  digitalWrite(HotPin, LOW);
  digitalWrite(ColdPin, LOW);
  analogWrite(PWM_Pin, 0);

  ads.begin();
  lcd.clear();

}
void loop(void)
{
  int16_t adc0; // we read from the ADC, we have a sixteen bit integer as a result

  adc0 = ads.readADC_SingleEnded(0); // Read ADC value from ADS1115

  Voltage = adc0 * (5.0 / 65535); // Replace 5.0 with whatever the actual Vcc of your Arduino is

  float resistance = (Voltage / refCurrent); // Using Ohm's Law to calculate resistance of thermistor

  float ln = log(resistance / thermistor_25); // Log of the ratio of thermistor resistance and resistance at 25 deg. C

  float kelvin = 1 / (0.0033540170 + (0.00025617244 * ln) + (0.0000021400943 * ln * ln) + (-0.000000072405219 * ln * ln * ln)); // Using the Steinhart-Hart Thermistor Equation to calculate temp in K

  float TempProm = kelvin - 273.15 - 0.68; // Converting Kelvin to Celcuis -0.610 is offset set through calibration factor


  Serial.print("Temperature ");
  Serial.print(TempProm);
  Serial.println(" *C");

  lcd.setCursor(0, 0);
  lcd.print("Temp      ");
  lcd.setCursor(10, 0);
  lcd.print(TempProm, 2);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Set Temp  ");
  lcd.setCursor(10, 0);
  lcd.print(Setpoint, 2);
  lcd.print("C");

  delay(200);
  if (digitalRead(up) == LOW) {
    Setpoint++;

    store();
    delay(100);
  }
  if (digitalRead(down) == LOW) {
    Setpoint--;

    store();
    delay(50);
  }

  myPID.SetTunings(consKp, consKi, consKd);

  myPID.Compute();

  if (Output > 0)  {

    digitalWrite(HotPin, HIGH);
    digitalWrite(ColdPin, LOW);
    analogWrite(PWM_Pin, Output);
    Serial.print("Hot_PWM  ");
    Serial.println(Output);
  }

  else
  {

    myPID.SetTunings(consKp, consKi, consKd);

    myPID.Compute();
    delay(200);
    digitalWrite(HotPin, LOW);
    digitalWrite(ColdPin, HIGH);
    analogWrite(PWM_Pin, -1 * Output);

    Serial.print("Cold_PWM  ");
    Serial.println(PWM_Pin);
  }

}

void store() {
  save_val = Setpoint * 100;
  mem[0] = save_val >> 24;
  mem[1] = save_val >> 16;
  mem[2] = save_val >> 8;
  mem[3] = save_val;

  //update memory
  EEPROM.update(address, mem[0]);
  EEPROM.update(address + 1, mem[1]);
  EEPROM.update(address + 2, mem[2]);
  EEPROM.update(address + 3, mem[3]);
}
