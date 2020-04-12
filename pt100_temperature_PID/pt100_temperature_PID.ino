#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MAX31865.h>
#include <PID_v1.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

//this library is for lookup table and for a accuracy increase to 0.001
#include <pt100rtd.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2, 4, 7, 8, 9, A1);

const int up_key = 6;
const int down_key = 5;
//float Voltage = 0.0;
//int thermistor_25 = 10000;
//float refCurrent = 0.001;

//Define Variables to connect to
double Setpoint, Average, Output, TempProm;
int save_val;

double consKp = 1, consKi = 0.25, consKd = 0.05;

//Specify the links and initial tuning parameters
PID myPID(&TempProm, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);


int PWM_Pin = 3;
int HotPin = A3;
int ColdPin = A4;
//memory storage
//byte mem[4];
//int address = 0;
//#define TempPin A0

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

// Like, duh.
#define C2F(c) ((9 * c / 5) + 32)

// init the Pt100 table lookup module
pt100rtd PT100 = pt100rtd() ;


//set offset here
double offset = 2.2;

void setup()
{
  Serial.begin(115200);
  //LM35 setup
  save_val =  read_string(100, 0).toInt();
  Setpoint = save_val / 100; // in ºC
  Serial.print("EEPROM Value");
  Serial.println(Setpoint);


  //initialize the variables
  thermo.begin(MAX31865_2WIRE);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);


  //__pinmode
  pinMode(PWM_Pin, OUTPUT);
  pinMode(HotPin, OUTPUT);
  pinMode(ColdPin, OUTPUT);
  pinMode(up_key, INPUT);
  pinMode(down_key, INPUT);
  myPID.SetOutputLimits(-255, 255);

  // Initialize Pins
  digitalWrite(HotPin, LOW);
  digitalWrite(ColdPin, LOW);
  digitalWrite(up_key, HIGH);
  digitalWrite(down_key, HIGH);
  analogWrite(PWM_Pin, 0);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); //Move coursor to second Line
  lcd.print("PID TempC v0.1");
  delay(200);


}

void loop(void)
{

  uint16_t rtd, ohmsx100 ;
  uint32_t dummy ;
  float ohms, Tlut ;  
  float Tcvd;

  //reading RTD
  rtd = thermo.readRTD();

  // fast integer math:
  // fits in 32 bits as long as (100 * RREF) <= 2^16,
  //  i.e., RREF must not exceed 655.35 ohms (heh).
  // TO DO: revise for 4000 ohm reference resistor needed by Pt1000 RTDs
 
  // Use uint16_t (ohms * 100) since it matches data type in lookup table.
  dummy = ((uint32_t)(rtd << 1)) * 100 * ((uint32_t) floor(RREF)) ;
  dummy >>= 16 ;
  ohmsx100 = (uint16_t) (dummy & 0xFFFF) ;

  // or use exact ohms floating point value.
  ohms = (float)(ohmsx100 / 100) + ((float)(ohmsx100 % 100) / 100.0) ;

  Serial.print("rtd: 0x") ; Serial.print(rtd,HEX) ;
  Serial.print(", ohms: ") ; Serial.println(ohms,8) ;

  TempProm = PT100.celsius(ohmsx100) ;     // NoobNote: LUT== LookUp Table
  Serial.print("Temperature = "); Serial.println(TempProm, 3); // temperature which is for sensing

  // Check and print any faults
  checkFault() ;

  //offset setting
  //TempProm -= offset;

  // Serial.print("Temperature ");
  // Serial.print(TempProm, 2);
  // Serial.println(" *C");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp = ");
  lcd.setCursor(8, 0);
  lcd.print(TempProm, 2);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Set Pt= ");
  lcd.print(Setpoint, 2);
  lcd.print(" C");
  delay(500);

  if (digitalRead(down_key) == LOW)
  {
    if (Setpoint > -40)
    {
      Setpoint -= 0.1;
    }
  }
  if (digitalRead(up_key) == LOW)
  {
    if (Setpoint < 450)
    {
      Setpoint += 0.1;
    }
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
    delay(100);
    digitalWrite(HotPin, LOW);
    digitalWrite(ColdPin, HIGH);
    analogWrite(PWM_Pin, -1 * Output);

    Serial.print("Cold_PWM  ");
    Serial.println(PWM_Pin);
  }
  Serial.print("Set point");
  Serial.println(Setpoint);
  save_val = Setpoint * 100;
  ROMwrite(String(save_val));
}

//----------Write to ROM-----------//
void ROMwrite(String temp) {
  temp += ";";
  write_EEPROM(temp, 0);
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

void checkFault(void)
{
  // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault)
  {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH)
    {
      Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH)
    {
      Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW)
    {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH)
    {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW)
    {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_OVUV)
    {
      Serial.println("Under/Over voltage");
    }
    thermo.clearFault();
  }
}
