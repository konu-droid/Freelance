#include <max6675.h>
#include <Wire.h>
#include <PID_v1.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>


//set offset here
double offset = 0 ;

int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int up_key = A0;
const int down_key = A1;

#define PWM_Pin 13

// make a cute degree symbol
uint8_t degree[8]  = {140, 146, 146, 140, 128, 128, 128, 128};

//Define Variables to connect to
double Setpoint, Output, T;
int save_val;

double consKp = 40 , consKi = 10.05, consKd = 10.25; //kpid 1,0.25,0.25 +-0.1

//Specify the links and initial tuning parameters
PID myPID(&T, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

void setup() {
  Serial.begin(9600);

  save_val =  read_string(100, 0).toInt();
  Setpoint = save_val / 100; // in ºC
  Serial.print("EEPROM Value");
  Serial.println(Setpoint   );

  // use Arduino pins
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);

  lcd.begin(16, 2);
  lcd.createChar(0, degree);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  //__pinmode
  pinMode(PWM_Pin, OUTPUT);
  pinMode(up_key, INPUT);
  pinMode(down_key, INPUT);
  myPID.SetOutputLimits(-255, 255);

  // Initialize Pins
  digitalWrite(up_key, HIGH);
  digitalWrite(down_key, HIGH);
  digitalWrite(PWM_Pin, 0);

  //LCD display
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); //Move coursor to second Line
  lcd.print("PID TEMP - 0.1 tested ");
  delay(100);

  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {

 
  T = thermocouple.readCelsius();

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

    analogWrite(PWM_Pin, Output);
    Serial.print("Hot_PWM                               = ");
    Serial.println(Output);
  }

  else
  {

    myPID.SetTunings(consKp, consKi, consKd);

    myPID.Compute();
    
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
