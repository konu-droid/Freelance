#include <Wire.h>
#include <PID_v1.h>
#include <EEPROM.h>
#include <SevSeg.h>

SevSeg Temp_sevseg;
SevSeg Set_sevseg;

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

#define Relay_Pin 13
#define HotPin 14
#define coldPin 15


void setup() {
  byte numDigits = 4;
  byte Temp_digitPins[] = {16, 17, 18, 19};
  byte Set_digitPins[] = {20, 21, 22, 23};
  byte segmentPins[] = {0, 1, 2, 3, 4, 10, 11, 12};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  Temp_sevseg.begin(hardwareConfig, numDigits, Temp_digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  Temp_sevseg.setBrightness(90);

  Set_sevseg.begin(hardwareConfig, numDigits, Set_digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  Set_sevseg.setBrightness(90);
  

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

  int t_0 = int(T*10);
  Temp_sevseg.setNumber(t_0, 1);

  
  }
  
  int s_0 = int(Setpoint*10);
  Set_sevseg.setNumber(s_0, 1);

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
  Temp_sevseg.refreshDisplay();
  Set_sevseg.refreshDisplay();

}
