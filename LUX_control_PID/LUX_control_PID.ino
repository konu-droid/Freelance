#include <PID_v1.h>
#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int up_key = 35;
const int down_key = 37;

//Define Variables to connect to
double Setpoint, Average, Output, CurVal;
int save_val;

double consKp = 1, consKi = 0.25, consKd = 0.05;

//Specify the links and initial tuning parameters
PID myPID(&CurVal, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

// Defining output pwm's
int PWM_Pin2 = 2;
int PWM_Pin3 = 3;
int PWM_Pin4 = 4;
int PWM_Pin5 = 5;
int PWM_Pin6 = 6;
int PWM_Pin7 = 7;
int PWM_Pin8 = 8;
int PWM_Pin9 = 9;
int PWM_Pin10 = 10;
int PWM_Pin11 = 11;
int PWM_Pin12 = 12;
int PWM_Pin13 = 13;
int PWM_Pin14 = 44;
int PWM_Pin15 = 45;
int PWM_Pin16 = 46;

// Defining Analog inputs
int IN_0 = A0;
int IN_1 = A1;
int IN_2 = A2;
int IN_3 = A3;
int IN_4 = A4;
int IN_5 = A5;
int IN_6 = A6;
int IN_7 = A7;
int IN_8 = A8;
int IN_9 = A9;
int IN_10 = A10;
int IN_11 = A11;
int IN_12 = A12;
int IN_13 = A13;
int IN_14 = A14;

//set offset here
//double offset = 2.2;

//Max value for calib
double max_val = 488;
double min_val = 1;

//Max PWM value
int max_PWM = 3825;

//min val for LDR
double max_val_l = 1023;   //put here the value of LDR, when LUX meter is 488 ( max of the current light source using )
double min_val_l = 0;      //put here the value of LDR, when LUX meter is 0

void setup()
{
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  //Read from the memory
  save_val =  read_string(100, 0).toInt();
  Setpoint = save_val / 100;
  Serial.print("EEPROM Value");
  Serial.println(Setpoint);


  //turn the PID on
  myPID.SetMode(AUTOMATIC);


  //__pinmode
  pinMode(PWM_Pin2, OUTPUT);
  pinMode(PWM_Pin3, OUTPUT);
  pinMode(PWM_Pin4, OUTPUT);
  pinMode(PWM_Pin5, OUTPUT);
  pinMode(PWM_Pin6, OUTPUT);
  pinMode(PWM_Pin7, OUTPUT);
  pinMode(PWM_Pin8, OUTPUT);
  pinMode(PWM_Pin9, OUTPUT);
  pinMode(PWM_Pin10, OUTPUT);
  pinMode(PWM_Pin11, OUTPUT);
  pinMode(PWM_Pin12, OUTPUT);
  pinMode(PWM_Pin13, OUTPUT);
  pinMode(PWM_Pin14, OUTPUT);
  pinMode(PWM_Pin15, OUTPUT);
  pinMode(PWM_Pin16, OUTPUT);

  pinMode(IN_0, INPUT);
  pinMode(IN_1, INPUT);
  pinMode(IN_2, INPUT);
  pinMode(IN_3, INPUT);


  pinMode(up_key, INPUT);
  pinMode(down_key, INPUT);

  myPID.SetOutputLimits(-max_PWM, max_PWM);

  // Initialize Pins
  digitalWrite(up_key, HIGH);
  digitalWrite(down_key, HIGH);

  analogWrite(PWM_Pin2, 0);
  analogWrite(PWM_Pin3, 0);
  analogWrite(PWM_Pin4, 0);
  analogWrite(PWM_Pin5, 0);
  analogWrite(PWM_Pin6, 0);
  analogWrite(PWM_Pin7, 0);
  analogWrite(PWM_Pin8, 0);
  analogWrite(PWM_Pin9, 0);
  analogWrite(PWM_Pin10, 0);
  analogWrite(PWM_Pin11, 0);
  analogWrite(PWM_Pin12, 0);
  analogWrite(PWM_Pin13, 0);
  analogWrite(PWM_Pin14, 0);
  analogWrite(PWM_Pin15, 0);
  analogWrite(PWM_Pin16, 0);

  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("PID LUX v0.1");
  delay(1500);

  lcd.clear();
}

void loop(void)
{
  CurVal = analogRead(IN_0);
  // CurVal += analogRead(IN_1);
  //CurVal += analogRead(IN_2);
  //CurVal += analogRead(IN_3);
  Serial.print("Value of LDR = ");
  Serial.println(CurVal);

  CurVal = map(CurVal, min_val_l, max_val_l, min_val, max_val);

  Serial.print("Value of LUX = ");
  Serial.println(CurVal);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(CurVal,1);
  lcd.setCursor(17, 0);
  lcd.print("Lx");

  lcd.setCursor(0, 1);
  lcd.print(Setpoint,1);
  lcd.setCursor(14, 1);
  lcd.print("Set Lx");

  delay(100);

  if (digitalRead(down_key) == LOW)
  {
    if (Setpoint > 0)
    {
      Setpoint -= 1;
    }
  }
  if (digitalRead(up_key) == LOW)
  {
    if (Setpoint < max_val)
    {
      Setpoint += 1;
    }
  }

  myPID.SetTunings(consKp, consKi, consKd);

  myPID.Compute();
  Serial.print("Value of PWM = ");
  Serial.println(Output);

  if (Output > 0)  {

    if (Output < 255) Set_out(Output, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    else if (Output < 510) Set_out(255, Output - 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    else if (Output < 765) Set_out(255, 255, Output - 510, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    else if (Output < 1020) Set_out(255, 255, 255, Output - 765, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    else if (Output < 1275) Set_out(255, 255, 255, 255, Output - 1020, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    else if (Output < 1560) Set_out(255, 255, 255, 255, 255, Output - 1275, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    else if (Output < 1785) Set_out(255, 255, 255, 255, 255, 255, Output - 1560, 0, 0, 0, 0, 0, 0, 0, 0);
    else if (Output < 2040) Set_out(255, 255, 255, 255, 255, 255, 255, Output - 1785, 0, 0, 0, 0, 0, 0, 0);
    else if (Output < 2295) Set_out(255, 255, 255, 255, 255, 255, 255, 255, Output - 2040, 0, 0, 0, 0, 0, 0);
    else if (Output < 2550) Set_out(255, 255, 255, 255, 255, 255, 255, 255, 255, Output - 2295, 0, 0, 0, 0, 0);
    else if (Output < 2805) Set_out(255, 255, 255, 255, 255, 255, 255, 255, 255, 255, Output - 2550, 0, 0, 0, 0);
    else if (Output < 3060) Set_out(255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, Output - 2805, 0, 0, 0);
    else if (Output < 3315) Set_out(255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, Output - 3060, 0, 0);
    else if (Output < 3570) Set_out(255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, Output - 3315, 0);
    else Set_out(255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, Output - 3570);

  }

  else
  {

    myPID.SetTunings(consKp, consKi, consKd);
    myPID.Compute();
    delay(100);
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


void Set_out(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n, int o) {

  analogWrite(PWM_Pin2, a);
  analogWrite(PWM_Pin3, b);
  analogWrite(PWM_Pin4, c);
  analogWrite(PWM_Pin5, d);
  analogWrite(PWM_Pin6, e);
  analogWrite(PWM_Pin7, f);
  analogWrite(PWM_Pin8, g);
  analogWrite(PWM_Pin9, h);
  analogWrite(PWM_Pin10, i);
  analogWrite(PWM_Pin11, j);
  analogWrite(PWM_Pin12, k);
  analogWrite(PWM_Pin13, l);
  analogWrite(PWM_Pin14, m);
  analogWrite(PWM_Pin15, n);
  analogWrite(PWM_Pin16, o);
}
