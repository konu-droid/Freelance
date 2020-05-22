#include <Wire.h>
#include <PID_v1.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "HX711.h"    //https://github.com/bogde/HX711


//set offset here
double offset = 0 ;


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
char buf[17];


#define UMIN  900000
#define UMAX 8000000
#define RMIN    60.0
#define RMAX   250.0


const int up_key = 6;
const int down_key = 5;


const long  Uu = 1834661;    // Raw reading lower end
const long  Uo = 2205459;    // Raw upper end reading
const float Ru = 100.01;  // Resistance value lower end
const float Ro = 123.24;  // Upper end resistance value

//Define Variables to connect to
double Setpoint, Average, Output, T;
int save_val;

double consKp = 40 , consKi = 10.05, consKd = 10.25; //kpid 1,0.25,0.25 +-0.1

//Specify the links and initial tuning parameters
PID myPID(&T, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);


#define PWM_Pin 3
#define HotPin A3
#define ColdPin A4

long Umess;
float Rx;


HX711 get_U;


void setup() {

  int i;
  long buf = 0;
  long U = 0;

  Serial.begin(115200);

  save_val =  read_string(100, 0).toInt();
  Setpoint = save_val / 100; // in ºC
  Serial.print("EEPROM Value");
  Serial.println(Setpoint   );

  Serial.println(F("HX711 Temperature measurement with Pt100 resistance thermometer"));
  get_U.begin(A1, A0, 32);

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

  //LCD display
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); //Move coursor to second Line
  lcd.print("PID TEMP - 0.1 tested ");
  delay(100);


}

void loop() {

  float k1, k2, k3, k4, k5, k6, k7, k8, k9;

  //Read in the measured value for the voltage at Rx
  Umess = get_U.read();

  // Calculate the resistance Rx from the measured voltage
  if (Umess >= UMIN && Umess <= UMAX) {
    Rx = ((((Ro - Ru) / (Uo - Uu)) * (Umess - Uu)) + Ru );
    //Serial.print("Umess = "); Serial.print(Umess); Serial.print("  ");
    Serial.print("R = ");
    Serial.print(Rx, 3);
    Serial.print(" Ohm   ->   ");

    lcd.setCursor(9, 0);
    lcd.print(Rx, 2);
    lcd.print("Ohm");

    // Calculate the temperature for Rx> = 100 Ohm
    if (Rx >= 100.0) {

      k1 = 3.90802 * pow(10, -1);
      k2 = 2 * 5.802 * pow(10, -5);
      k3 = pow(3.90802 * pow(10, -1), 2);
      k4 = 4.0 * (pow(5.802 * pow(10, -5), 2));
      k5 = Rx - 100.0;
      k6 = 5.802 * pow(10, -5);

      k7 = k1 / k2;
      k8 = (k3 / k4) - (k5 / k6);
      k9 = sqrt(k8);

      T = k7 - k9;

      T = (-4 * sqrt(-57750000 * Rx + 43956550801) + 781604) / 231;

    }

    // Calculate the temperature for Rx <100 Ohm
    else {

      k1 = pow (Rx, 5) * 1.597 * pow(10, -10);
      k2 = pow (Rx, 4) * 2.951 * pow(10, -8);
      k3 = pow (Rx, 3) * 4.784 * pow(10, -6);
      k4 = pow (Rx, 2) * 2.613 * pow(10, -3);
      k5 = 2.219 * Rx - 241.9;

      T = k1 - k2 - k3 + k4 + k5;

      T = (4 * sqrt(-57750000 * Rx + 43956550801) + 781604) / 231;

    }

    Serial.print("T = ");
    Serial.print(T, 3);
    Serial.println("C");

  }

  else {
    Serial.println("Measurement error");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error");
  }

  // ==========================****=========================

  //offset setting
  T += offset;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp  = ");
  lcd.setCursor(9, 0);
  lcd.print(T, 1);
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


  Serial.print("Set point                                                = ");
  Serial.println(Setpoint);
  save_val = Setpoint * 100;
  ROMwrite(String(save_val));


  delay(500);
}


char *ftoa(char *a, double f, int precision)
{
  long p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
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
