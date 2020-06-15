#include <Wire.h>
#include <PID_v1.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "HX711.h"    //https://github.com/bogde/HX711


//set offset here
double offset = 0 ;


// Connect via i2c, default address #0 (A0-A2 not jumpered)
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
char buf[17];


#define UMIN  900000
#define UMAX 8000000
#define RMIN    60.0
#define RMAX   250.0


const int up_key = 5;
const int down_key = 4;


const long Uu = 2341211; // Raw reading lower end422 -40
const long Uo = 5730094; // Raw upper end reading682 +300
const float Ru = 84.27; // Resistance value lower end
const float Ro = 212.01; // Upper end resistance value.0188

//Define Variables to connect to
double Setpoint, Output, T;

unsigned long timeTaken = 0;
unsigned long startTime = 0;
unsigned long tempTime = 0;

double consKp = 2, consKi = 1, consKd =  35; //kpid 1,0.25,0.25 +-0.1//Kp=2, Ki=5, Kd=1;now  2,0.01,1    0.5, 0.01,20,  0.5, 0.01,2   1, 0.01,2 in oven 0.5,0.01,10 ,0.5,0.01,6  2510,2140

//Specify the links and initial tuning parameters
PID myPID(&T, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

int windowSize = 200;//900,1500,500,200

//temp read delay
int temp_read_delay = 500;

#define Relay_Pin 6
#define HotPin 3
#define coldPin 2

long Umess;
float Rx;


HX711 get_U;


void setup() {

  int i;
  long buf = 0;
  long U = 0;

  Serial.begin(115200);

  EEPROM.get(1, Setpoint);
  Serial.print("EEPROM Value");
  Serial.println(Setpoint   );

  Serial.println(F("HX711 Temperature measurement with Pt100 resistance thermometer"));
  get_U.begin(A1, A0, 32);


  //__pinmode
  pinMode(Relay_Pin, OUTPUT);
  pinMode(HotPin, OUTPUT);
  pinMode(coldPin, OUTPUT);
  pinMode(up_key, INPUT);
  pinMode(down_key, INPUT);

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, windowSize); //-10000,-9000(-10000, windowSize)

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  // Initialize Pins
  digitalWrite(HotPin, LOW);
  digitalWrite(up_key, HIGH);
  digitalWrite(down_key, HIGH);
  digitalWrite(Relay_Pin, LOW);

  //LCD display
  lcd.begin(20, 4);
  lcd.setCursor(0, 0); //Move coursor to second Line
  lcd.print("PID-TEMP-0.2");
  delay(100);


}

void loop() {

  float k1, k2, k3, k4, k5, k6, k7, k8, k9;

  timeTaken = millis();

  if (timeTaken - tempTime > temp_read_delay) {

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
      Serial.print(T, 2);
      Serial.println("C");

    }

    else {
      Serial.println("Measurement error");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Error");
    }

    tempTime = timeTaken;
  }

  // ==========================****=========================

  //offset setting
  T += offset;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp  = ");
  lcd.setCursor(9, 0);
  lcd.print(T, 2);
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
    if (Setpoint < 70)
    {
      Setpoint += 0.1;
    }
  }

  myPID.SetTunings(consKp, consKi, consKd);



  /************************************************
     turn the output pin on/off based on pid output
   ************************************************/

  timeTaken = millis();

  if (timeTaken - startTime > windowSize) {
    myPID.Compute();
    startTime = millis();
  }

  bool set;
  if ( T - Setpoint <= 0 ) set = 0;
  else if ( T - Setpoint > 0.5 ) set = 1;

  if (set == 1) digitalWrite(coldPin, HIGH);
  else digitalWrite(coldPin, LOW);
  
  if (timeTaken - startTime < Output) digitalWrite(Relay_Pin, HIGH);
  else digitalWrite(Relay_Pin, LOW);


  digitalWrite(HotPin, HIGH);
  Serial.print("relay value        = ");
  Serial.println(Output);

  /*

    if (Setpoint == T )  {

      digitalWrite(HotPin, LOW);
      Serial.print("OK");
    }

    else if (Setpoint - T > 0) {

      digitalWrite(HotPin, HIGH);
      Serial.print("Hot_PIN");

    }

    else
    {

      digitalWrite(HotPin, LOW);
      Serial.print("Cold");

    }

  */

  Serial.print("Set point                                                = ");
  Serial.println(Setpoint);
  EEPROM.put(1, Setpoint);

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
