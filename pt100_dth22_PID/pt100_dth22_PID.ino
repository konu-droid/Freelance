/*
   this uses boht pt100 for accurate temperature and a dth22 module for humidity
   uses PWM outputs to control the dehumidifier, heater, cooler and wetter devices
*/

#include <Adafruit_MAX31865.h>
#include <PID_v1.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "DHT.h"

#define DHTPIN 7     // what pin we're connected to 2

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)


DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display


const int up_key = 2;
const int down_key = 3;
const int ok = 4;
//float Voltage = 0.0;
//int thermistor_25 = 10000;
//float refCurrent = 0.001;

//Define Variables to connect to
double Setpoint_temp, Output1, TempProm;
double Setpoint_hum, Output2, HumProm;
int save_val;

double consKp = 1, consKi = 0.25, consKd = 0.05;

//Specify the links and initial tuning parameters
PID myPID_temp(&TempProm, &Output1, &Setpoint_temp, consKp, consKi, consKd, DIRECT);
PID myPID_hum(&HumProm, &Output2, &Setpoint_hum, consKp, consKi, consKd, DIRECT);


int PWM_Pin_temp = 5;
int PWM_Pin_hum = 6;

bool flag = 0;

// define pins for relay or leds
#define heater A3
#define wetter A2
#define cooler A1
#define dryer A0


byte grad[8] = {
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
};

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

void setup()
{
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);

  save_val =  read_string(100, 0).toInt();
  Setpoint_temp = save_val / 100; // in ºC
  Serial.print("EEPROM Value Temp");
  Serial.println(Setpoint_temp);

  save_val =  read_string(100, 100).toInt();
  Setpoint_hum = save_val / 100;
  Serial.print("EEPROM Value Hum");
  Serial.println(Setpoint_hum);


  //initialize the variables
  thermo.begin(MAX31865_2WIRE);

  // Initialize DHT sensor
  dht.begin();

  //turn the PID on
  myPID_temp.SetMode(AUTOMATIC);
  myPID_hum.SetMode(AUTOMATIC);


  //PWM pins
  pinMode(PWM_Pin_temp, OUTPUT);
  pinMode(PWM_Pin_hum, OUTPUT);


  // define outputs
  pinMode(heater, OUTPUT);
  pinMode(wetter, OUTPUT);
  pinMode(cooler, OUTPUT);
  pinMode(dryer, OUTPUT);

  // set the default state for outputs
  digitalWrite(heater, LOW);
  digitalWrite(wetter, LOW);
  digitalWrite(cooler, LOW);
  digitalWrite(dryer, LOW);

  // define control buttons
  pinMode(up_key, INPUT);
  pinMode(down_key, INPUT);
  pinMode(ok, INPUT);

  myPID_temp.SetOutputLimits(-255, 255);
  myPID_hum.SetOutputLimits(-255, 255);

  // Initialize Pins
  digitalWrite(up_key, HIGH);
  digitalWrite(down_key, HIGH);
  digitalWrite(ok, HIGH);

  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("PID TempC &");
  lcd.setCursor(0, 1);
  lcd.print("Humidity v0.1");
  delay(200);

  // set the default state for outputs
  digitalWrite(heater, LOW);
  digitalWrite(wetter, LOW);
  digitalWrite(cooler, LOW);
  digitalWrite(dryer, LOW);
  analogWrite(PWM_Pin_temp, 0);
  analogWrite(PWM_Pin_hum, 0);

  lcd.clear();

}

void loop(void)
{
  uint16_t rtd = thermo.readRTD();

  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  Serial.print("Ratio = "); Serial.println(ratio, 8);
  Serial.print("Resistance = "); Serial.println(RREF * ratio, 8);

  TempProm = thermo.temperature(RNOMINAL, RREF);
  Serial.print("Temperature = "); Serial.println(TempProm, 2); // temperature which is for sensing

  // Check and print any faults
  check_fault();

  // Read Humidity
  HumProm = dht.readHumidity();

  //display temp and hum
  lcd.setCursor(0, 0);
  lcd.print("Current Temp = ");
  lcd.setCursor(15, 0);
  lcd.print(TempProm, 1);
  lcd.write(byte(0));
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Set Temp     = ");
  lcd.setCursor(15, 1);
  lcd.print(Setpoint_temp, 1);
  lcd.write(byte(0));
  lcd.print("C");

  lcd.setCursor(0, 2);
  lcd.print("Current Humi = ");
  lcd.setCursor(17, 2);
  lcd.print(HumProm, 0);
  lcd.print("%");

  lcd.setCursor(0, 3);
  lcd.print("Set Humidity = ");
  lcd.setCursor(17, 3);
  lcd.print(Setpoint_temp, 0);
  lcd.print("%");

  delay(500);

  if (flag == 1) {
    if (digitalRead(down_key) == LOW)
    {
      if (Setpoint_temp > -40)
      {
        Setpoint_temp -= 0.1;
      }
    }
    if (digitalRead(up_key) == LOW)
    {
      if (Setpoint_temp < 450)
      {
        Setpoint_temp += 0.1;
      }
    }
  }
  else {
    if (digitalRead(down_key) == LOW)
    {
      if (Setpoint_hum > 0)
      {
        Setpoint_hum --;
      }
    }
    if (digitalRead(up_key) == LOW)
    {
      if (Setpoint_hum < 100)
      {
        Setpoint_hum ++;
      }
    }
  }

  if(digitalRead(ok) == LOW) flag = !flag;

  myPID_temp.SetTunings(consKp, consKi, consKd);
  myPID_hum.SetTunings(consKp, consKi, consKd);

  myPID_temp.Compute();
  myPID_hum.Compute();


  // Set the PWM outputs using PID
  if (Output1 > 0)  {

    digitalWrite(heater, HIGH);
    digitalWrite(cooler, LOW);
    analogWrite(PWM_Pin_temp, Output1);
    Serial.print("Hot_PWM  ");
    Serial.println(Output1);
  }
  else
  {

    myPID_temp.SetTunings(consKp, consKi, consKd);

    myPID_temp.Compute();
    delay(100);
    digitalWrite(heater, LOW);
    digitalWrite(cooler, HIGH);
    analogWrite(PWM_Pin_temp, -1 * Output1);

    Serial.print("Cold_PWM  ");
    Serial.println(-1 * Output1);
  }

  if (Output2 > 0)  {

    digitalWrite(wetter, HIGH);
    digitalWrite(dryer, LOW);
    analogWrite(PWM_Pin_hum, Output2);
    Serial.print("Hot_PWM  ");
    Serial.println(Output2);
  }
  else
  {

    myPID_hum.SetTunings(consKp, consKi, consKd);

    myPID_hum.Compute();
    delay(100);
    digitalWrite(wetter, LOW);
    digitalWrite(dryer, HIGH);
    analogWrite(PWM_Pin_hum, -1 * Output2);

    Serial.print("Cold_PWM  ");
    Serial.println(-1 * Output2);
  }

  //update eeprom value if it changes
  Serial.print("Set point temp");
  Serial.println(Setpoint_temp);
  save_val = Setpoint_temp * 100;
  ROMwrite(String(save_val), 0);

  Serial.print("Set point hum");
  Serial.println(Setpoint_hum);
  save_val = Setpoint_hum * 100;
  ROMwrite(String(save_val), 100);

}

//----------Write to ROM-----------//
void ROMwrite(String temp, int pos) {
  temp += ";";
  write_EEPROM(temp, pos);
  //EEPROM.commit();
}

void ROMwriteNode(String node, int pos) {
  node += ";";
  write_EEPROM(node, pos);
  //EEPROM.commit();
}


//****Write to ROM***//
void write_EEPROM(String x, int pos) {
  for (int n = pos; n < x.length() + pos; n++) {
    //write the ssid and password fetched from webpage to EEPROM
    EEPROM.update(n, x[n - pos]);
  }
}


//*********EEPROM Read*********//
String read_string(int l, int p) {
  String temp;
  for (int n = p; n < l + p; ++n)
  {
    // read the saved password from EEPROM
    if (char(EEPROM.read(n)) != ';') {

      temp += String(char(EEPROM.read(n)));
    } else n = l + p;
  }
  return temp;
}

void check_fault() {
  // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage");
    }
    thermo.clearFault();
  }

}
