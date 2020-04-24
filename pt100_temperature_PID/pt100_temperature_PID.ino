#include <Adafruit_MAX31865.h>
#include <PID_v1.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
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
//double aggKp=4, aggKi=0.2, aggKd=1;
double consKp = 40 , consKi = 10.05, consKd = 10.25; //kpid 1,0.25,0.25 +-0.1

//Specify the links and initial tuning parameters
PID myPID(&TempProm, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);


#define PWM_Pin 3
#define HotPin A3
#define ColdPin A4
#define BuzzerPin A5

int Timer = 0;
int delay_mins = 5;                              // in minutes
int Buzzer_on_time = 3000;                       // in milli seconds

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.00
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

//set offset here
double offset = 0 ;
void setup()
{
  Serial.begin(115200);
  //LM35 setup
  save_val =  read_string(100, 0).toInt();
  Setpoint = save_val / 100; // in ºC
  Serial.print("EEPROM Value");
  Serial.println(Setpoint   );


  //initialize the variables
  thermo.begin(MAX31865_4WIRE);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);


  //__pinmode
  pinMode(PWM_Pin, OUTPUT);
  pinMode(HotPin, OUTPUT);
  pinMode(ColdPin, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(up_key, INPUT);
  pinMode(down_key, INPUT);
  myPID.SetOutputLimits(-255, 255);

  // Initialize Pins
  digitalWrite(HotPin, LOW);
  digitalWrite(ColdPin, LOW);
  digitalWrite(BuzzerPin, LOW);
  digitalWrite(up_key, HIGH);
  digitalWrite(down_key, HIGH);
  analogWrite(PWM_Pin, 0);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); //Move coursor to second Line
  lcd.print("PID TEMP - 0.1 tested ");
  delay(100);


}

void loop(void)
{
  uint16_t rtd = thermo.readRTD();
  delay(100);
  // Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;//32768
  //Serial.print("Ratio = "); Serial.println(ratio, 8);
  Serial.print("Resistance = "); Serial.println(RREF * ratio, 8);
  TempProm = thermo.temperature(RNOMINAL, RREF);
  Serial.print("Temperature =              "); Serial.println(TempProm, 8); // temperature which is for sensing

  check_fault();

  //offset setting
  TempProm += offset;

  // Serial.print("Temperature ");
  // Serial.print(TempProm, 2);
  // Serial.println(" *C");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp  = ");
  lcd.setCursor(9, 0);
  lcd.print(TempProm, 1);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Set Pt = ");
  lcd.print(Setpoint, 1);
  lcd.print(" C");

  if (digitalRead(down_key) == LOW)

  {
    if (Setpoint > -30)
    {
      Setpoint -= 0.1;
    }
  }
  if (digitalRead(up_key) == LOW)
  {
    if (Setpoint < 60)
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
    Serial.print("Hot_PWM                               = ");
    Serial.println(Output);
  }

  else
  {

    myPID.SetTunings(consKp, consKi, consKd);

    myPID.Compute();

    digitalWrite(HotPin, LOW);
    digitalWrite(ColdPin, HIGH);
    analogWrite(PWM_Pin, -1 * Output);

    Serial.print("Cold_PWM                               = ");
    Serial.println(Output);
    
  }

  if (( TempProm - Setpoint == 0.1 ) || ( TempProm - Setpoint == (-0.1) )) {
    if ( Timer > delay_mins*600 ){
      
      digitalWrite(BuzzerPin,HIGH);
      delay(Buzzer_on_time);
      Timer = Buzzer_on_time;
      
      }
    }

  Timer++;
    
  Serial.print("Set point                                                = ");
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
    EEPROM.write(n, x[n - pos]);
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
    //  Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      //  Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      //Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      //  Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      // Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      //  Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_OVUV) {
      // Serial.println("Under/Over voltage");
    }
    thermo.clearFault();
  }
}
