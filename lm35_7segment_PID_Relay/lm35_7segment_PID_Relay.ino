/*
   Bahiya essko set value and real value seven
   segment per kardo and epprom, relay pid output
   with cooling pin output kar do and set value
   fast and real value delay wala functions add
   kar do and or output sab delt kar do only two
   output used one pid relay onoff for heating
  And one for cool (0.5+-)onoff
  Using lm35
*/

/*
  circuits4you.com
  Digital Temperature Controller
*/
#include <Wire.h>
#include <PID_v1.h>
#include <EEPROM.h>
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 13 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(12,13,10,1);

//set offset here
double offset = 0 ;

const int up_key = 5;
const int down_key = 4;

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


void setup() {

  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);

  Serial.begin(115200);

  EEPROM.get(1, Setpoint);
  Serial.print("EEPROM Value");
  Serial.println(Setpoint   );


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

  delay(100);
}

void loop() {

  timeTaken = millis();

  if (timeTaken - tempTime > temp_read_delay) {
    T = ((5.0 / 1024.0) * analogRead(A0)) * 100; //10mV per degree 0.01V/C. Scalling

    //offset setting
  T += offset;

  int t_0 = int(T*10)%10;
  int t_1 = int(T)%10;
  int t_2 = int(T/10)%10;
  int t_3 = int(T/100)%10;
  

  lc.setDigit(0,0,t_0,false);
  lc.setDigit(0,1,t_1,false);
  lc.setDigit(0,2,t_2,false);
  lc.setDigit(0,3,t_3,false);
  
  }
  
  int s_0 = int(Setpoint*10)%10;
  int s_1 = int(Setpoint)%10;
  int s_2 = int(Setpoint/10)%10;
  int s_3 = int(Setpoint/100)%10;

  lc.setDigit(0,4,s_0,false);
  lc.setDigit(0,5,s_1,false);
  lc.setDigit(0,6,s_2,false);
  lc.setDigit(0,7,s_3,false);

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
