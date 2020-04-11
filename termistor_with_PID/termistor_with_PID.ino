#include <PID_v1.h>

float Voltage = 0.0;
int thermistor_25 = 10000;
float refCurrent = 0.0001;

#define SAMPLES 5

int samples[SAMPLES];

//Define Variables to connect to
double Setpoint, Average, Output, TempProm;

double consKp = 1, consKi = 0.05, consKd = 0.25;

//Specify the links and initial tuning parameters
PID myPID(&TempProm, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

int PWM_Pin = 10;
int HotPin = 2;
int ColdPin = 7;

#define TempPin A0

void setup()
{
  Serial.begin(9600);
  //LM35 setup
  {

    //initialize the variables
    Average = analogRead(TempPin);
    Setpoint = 27; // in ºC

    //turn the PID on
    myPID.SetMode(AUTOMATIC);


    //__pinmode
    pinMode(PWM_Pin, OUTPUT);
    pinMode(HotPin, OUTPUT);
    pinMode(ColdPin, OUTPUT);

    myPID.SetOutputLimits(-255, 255);

    // Initialize Pins
    digitalWrite(HotPin, LOW);
    digitalWrite(ColdPin, LOW);
    analogWrite(PWM_Pin, 0);


  }
}
void loop(void)
{
  uint8_t i;

  // take N samples in a row, with a slight delay
  for (i = 0; i < SAMPLES; i++) {
    samples[i] = analogRead(TempPin);
    delay(10);
  }

  // average all the samples out
  Average = 0;
  for (i = 0; i < SAMPLES; i++) {
    Average += samples[i];
  }
  Average /= SAMPLES;

  Voltage = Average * (5.0 / 255);// convert to C

  float resistance = (Voltage / refCurrent); // Using Ohm's Law to calculate resistance of thermistor

  float ln = log(resistance / thermistor_25); // Log of the ratio of thermistor resistance and resistance at 25 deg. C

  float kelvin = 1 / (0.0033540170 + (0.00025617244 * ln) + (0.0000021400943 * ln * ln) + (-0.000000072405219 * ln * ln * ln)); // Using the Steinhart-Hart Thermistor Equation to calculate temp in K

  TempProm = kelvin - 273.15 - 0.68; // Converting Kelvin to Celcuis -0.610 is offset set through calibration factor


  Serial.print("Temperature ");
  Serial.print(TempProm);
  Serial.println(" *C");

  delay(1000);

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
    delay(1000);
    digitalWrite(HotPin, LOW);
    digitalWrite(ColdPin, HIGH);
    analogWrite(PWM_Pin, -1 * Output);

    Serial.print("Cold_PWM  ");
    Serial.println(PWM_Pin);
  }

}
