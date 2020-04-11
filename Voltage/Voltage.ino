/*
   The circuit: /////////////////////////LCD wiring is same as hello world //////
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 Pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   10K resistor:
   ends to +5v and ground
   Pot to LCD v0 pin (pin 3)
*/

#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;
const float multiplier = 0.02;

int H = 0;
int M = 0;
int S = 0;
int flag = 0;

//int analogInput0 = A0;                                   // port for sence voltage
int analogInput1 = A2;                                  //  port for measure current

double Volts = 0.0;
double vout = 0.0;
double vin = 0.0;
int value;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

double Power;
double PowerWh;

void setup()

{
  Serial.begin(9600);

  // Descomentar el que interese
  // ads.setGain(GAIN_TWOTHIRDS);  +/- 6.144V  1 bit = 0.1875mV (default)
  // ads.setGain(GAIN_ONE);        +/- 4.096V  1 bit = 0.125mV
  // ads.setGain(GAIN_TWO);        +/- 2.048V  1 bit = 0.0625mV
  // ads.setGain(GAIN_FOUR);       +/- 1.024V  1 bit = 0.03125mV
  // ads.setGain(GAIN_EIGHT);      +/- 0.512V  1 bit = 0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    +/- 0.256V  1 bit = 0.0078125mV
  ads.begin();

  //pinMode(analogInput0, INPUT);                               //A0 sets as input
  pinMode(analogInput1, INPUT);                              // A2 set as input

  lcd.begin(20, 4);

}

void loop()
{

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////                                                                                           /////////////////////
  ////////////////////   Voltage = (((Arduino measured analog reading) * (5/1024)) /(r2/r1+r2)) * (Power factor ) ////////////////////
  /////////////////////                                                                                           ////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  int16_t adc0, adc1, adc2, adc3;


  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  Serial.print("AIN0: "); Serial.println(adc0 * multiplier);
  Serial.print("AIN1: "); Serial.println(adc1 * multiplier);
  Serial.print("AIN2: "); Serial.println(adc2 * multiplier);
  Serial.print("AIN3: "); Serial.println(adc3 * multiplier);
  Serial.println(" ");
  lcd.clear();





  //value = analogRead(analogInput0);      // analogRead (analogInput0) Reads the value from the analog pin A0.
  //The Arduino board contains a 6 channel (8 channels on the Mini and Nano, 16 on the Mega),
  //10-bit analog to digital converter. This means that it will map input voltages between 0 and 5 volts into integer values between 0 and 1023.
  //This yields a resolution between readings of: 5 volts / 1024 units or, .0049 volts (4.9 mV) per unit. The input range and resolution can be
  //changed using analogReference().
  //It takes about 100 microseconds (0.0001 s) to read an analog input, so the maximum reading rate is about 10,000 times a second.
  // Voltage = (((Arduino measured analog reading) * (5/1024)) /(r2/r1+r2)) * (Power factor )

  //vout = (value)  *  (5 / 1024.0);

  Volts = adc0 * multiplier;    //formula for voltage divider  r2/(r1+r2)  actual value of calculation
  // is 0.90909090909090909090909090909091 but i changed its value to 0.999
  // for getting accrate voltage reading by compare it with DSO reading
  // power factor = output voltage / input voltage ,55/5=11 and here actual calculation is  power factor = 11.0447365


  if (Volts < 0)
  {
    Volts = 0;
  }



  ////////////////////////////////
  ///*current sensing formula*/////
  ////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////
  /////////                Measuring Current Using ACS712                    /////////
  ///Current = (AcsOffset - (Arduino measured analog reading)) / Sensitivity /////////
  ////////////////////////////////////////////////////////////////////////////////////




  {
    unsigned int X = 0;
    float AcsADCValue = 0.0, Samples = 0.0, AvgAcsADC = 0.0, AcsValueF = 0.0;
    for (int x = 0; x < 10; x++)
    {

      AcsADCValue = analogRead(A2);        // analogRead(A5)Reads the value from the  analog pin A5.
      //The Arduino board contains a 6 channel (8 channels on the Mini and Nano, 16 on the Mega),
      //10-bit analog to digital converter. This means that it will map input voltages between 0 and 5 volts into integer values between 0 and 1023.
      //This yields a resolution between readings of: 5 volts / 1024 units or, .0049 volts (4.9 mV) per unit. The input range and resolution can be
      //changed using analogReference().
      //It takes about 100 microseconds (0.0001 s) to read an analog input, so the maximum reading rate is about 10,000 times a second.
      Samples = Samples + AcsADCValue;

    }

    AvgAcsADC = (Samples / 10.0);           //taking avarage of 10 values , AvgAcsADC should be be 512 0r 511 ie, Arduino ADC reads  5 volt at A5 port as 1024
    //At zero current 2.5 volt will present in A5 port that means ADC of arduino reads it as 512 or 511 (5/2=2.5 and 1024/2=512).
    //if it is 510 or below result -ve value in current reading so for acorrection make the next de-activated line active with
    //the numeric numer 1 or 2 according to your sensor input.
    //AvgAcsADC+=1;


    AcsValueF = (AvgAcsADC * 0.0048828125); // AcsValueF is the Voltage reads by ACS 712 sensor. Actual logic is Acs712 reading * 5/1024 = voltage measured by Acs 712,  5/1024=0.0048828125.

    AcsValueF -= 2.5;                       // When  zero current flows through the sensor it shows a voltage of VCC/2 ie 5/2= 2.5v this known as offset voltage.
    //For getting value of current, current = offset voltage - voltage measured by  ACS sensor

    //float Current = AvgAcsADC;            // this code line is no need for current calculaton. i have mentioned it for get ADC value of A5 for most accurate calculation

    float Current = AcsValueF / .1;         //sensor ACS 712 20A  has sensitivity of 100mV  , Sensitivity of 30A is 66mV and sensitivity of 5A is 185 mV
    //use this value  the code with respect of your sensor current capacity.
    //Current = (AcsOffset - (Arduino measured analog reading)) / Sensitivity
    if (Current < 0)
    {
      Current = 0;
    }



    Power = (Volts * Current);                      // Calculation of Power
    PowerWh = (Power / 3600);                     //Calculation of watt/ hr , 3600 is the number of seconds in 1 Hr


    S = S + 1;                                     // Calculation of 24Hr elapsed Timer which will gives running time
    if (S == 60)
    {
      S = 0;
      M = M + 1;
    }
    lcd.clear();
    if (M == 60)
    {
      M = 0;
      H = H + 1;
      flag = flag + 1;
    }
    if (flag == 24)
    {
      flag = 0;
    }

    lcd.setCursor(0, 0);
    lcd.print("                  ");
    lcd.setCursor(0, 0);
    lcd.print(Volts, 1);
    lcd.print("V");

    lcd.setCursor(10, 0);
    lcd.print("                  ");
    lcd.setCursor(10, 0);
    lcd.print(Current, 1);
    lcd.print("Amp");

    lcd.setCursor(0, 1);
    lcd.print("                 ");
    lcd.setCursor(0, 1);
    lcd.print(Power, 1);
    lcd.print("W");

    lcd.setCursor(10, 1);
    lcd.print("                 ");
    lcd.setCursor(10, 1);
    lcd.print(PowerWh, 1);
    lcd.print(" W/hr");
    //delay(1000);

    lcd.setCursor(4, 2);
    lcd.print("Time Elapsed");

    lcd.setCursor(6, 3);
    lcd.print(H);
    lcd.print(":");
    lcd.print(M);
    lcd.print(":");
    lcd.print(S);

    delay(998);


  }
}
