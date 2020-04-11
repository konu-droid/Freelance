/*
 * This code uses a single DHT11 sensor to find the current temp and humidity
 * It uses a resistive type heating element and a cooler outputs to change the temperature of the room
 * created a menu to change the temperature and humidity along with setting there tolerances (since the heater is resistive type)
 * also saves the set values into the EEPROM and a 16*2 lcd to display, also has a extra reset button, has fake temp and also max and min of temp and humidity.
 */ 
 
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte grad[8] = {
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
};

#include "DHT.h"
#define DHTPIN A0    // what pin we're connected to A0
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)


DHT dht(DHTPIN, DHTTYPE);


#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 10
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//variabl for time
long a = 0;

//define 5 second press variables
long buttonTimer = 0, buttonTimer1 = 0;
long longPressTime = 100;

// define pins for relay or leds
#define heater 6
#define wetter 7
#define cooler 8
#define dryer 9
#define buzzer A4

//buzzer variable
boolean sound=0;

// define variable
float temp, t1, reqtemp, temphyst, minitemp = 150, maxitemp = -10, faketemp = 0; // variable for temperature
int hum, reqhum, humhyst, minihum = 200, maxihum = 200, fakehum = 0; // variable for humiditiy
byte heat;  // is 1 for heater and 0 for cooler
byte dry;   // is 1 for dryer and 0 for wetter

int BUT1 = A1;  // - switch/min temp
int BUT2 = A2; // + switch
int BUT3 =  A3; // MENU switch

byte meniu = 0; // if MENIU = 0 is clasical
// if MENIU = 1 is for temperature set (reqtemp)
// if MENIU = 2 is for dt temperature (temphyst)
// if MENIU = 3 is for humidity set (reqhum)
// if MENIU = 4 is for dh humidity (humhyst)
// if MENIU = 5 is for HEAT/COOL
// if MENIU = 6 id for DRY/WET

#include <EEPROM.h>


void setup()
{

  // Initialize LCD display
  lcd.begin(16, 2);
  // create custom symbol
  lcd.createChar(0, grad);

  // Initialize DHT sensor
  dht.begin();

  // for DS18B20 sensor
  sensors.begin();

  // define outputs
  pinMode(heater, OUTPUT);
  pinMode(wetter, OUTPUT);
  pinMode(cooler, OUTPUT);
  pinMode(dryer, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // set the default state for outputs
  digitalWrite(heater, LOW);
  digitalWrite(wetter, LOW);
  digitalWrite(cooler, LOW);
  digitalWrite(dryer, LOW);
  digitalWrite(dryer, LOW);

  // set push buttons for menu
  pinMode(BUT1, INPUT_PULLUP);
  pinMode(BUT2, INPUT_PULLUP);
  pinMode(BUT3, INPUT_PULLUP);

  lcd.setCursor(0, 0);
  lcd.print("thermostat with");

  lcd.setCursor(0, 1);
  lcd.print("humidity control");

  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("original sketch");

  lcd.setCursor(0, 1);
  lcd.print("ver.1.0  KONU ");
  delay(3000);
  lcd.clear();
  //inti
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  /*
    // just first time... after must put commnent (//)
    EEPROM.write(201,0);    // tset1
    EEPROM.write(202,245);  // tset2
    EEPROM.write(203,5);    // dt x 10
    EEPROM.write(204,45);   // reqhum
    EEPROM.write(205,5);    // humhyst
    EEPROM.write(206,1);    // is 1 for heater and 0 for cooler
    EEPROM.write(207,1);    // is 1 for dryer and 0 for wetter
    EEPROM.write(209,0);    //mini tset1
    EEPROM.write(210,245);    //mini tset2
    EEPROM.write(211,0);    //maxi tset1
    EEPROM.write(212,245);    //maxi tset2
    EEPROM.write(213,100);    //minihum
    EEPROM.write(214,0);    //maxihum
  */

  byte tset1 = EEPROM.read(201);
  byte tset2 = EEPROM.read(202);
  reqtemp = 256 * tset1 + tset2;  // recover the number
  reqtemp = reqtemp / 10;
  byte minitset1 = EEPROM.read(209);
  byte minitset2 = EEPROM.read(210);
  minitemp = 256 * minitset1 + minitset2;  // recover the number
  minitemp = minitemp / 10;
  byte maxitset1 = EEPROM.read(211);
  byte maxitset2 = EEPROM.read(212);
  maxitemp = 256 * maxitset1 + maxitset2;  // recover the number
  maxitemp = maxitemp / 10;
  temphyst = EEPROM.read(203);
  temphyst = temphyst / 10;
  reqhum = EEPROM.read(204);
  minihum = EEPROM.read(213);
  maxihum = EEPROM.read(214);
  humhyst = EEPROM.read(205);
  heat = EEPROM.read(206);
  dry = EEPROM.read(207);
}
void(* resetFunc) (void) = 0;//declare reset function at address 0

void loop()
{
  a = millis();
  if (meniu >= 9) meniu = 0;
  if (digitalRead(BUT3) == LOW)
  { meniu = meniu + 1;
    lcd.clear();
    delay(250);
  }


  if (meniu == 0)
  {

    //get sensor values
    hum = dht.readHumidity();
    temp = dht.readTemperature();

    //finding the mini and max
    if (temp < minitemp) {
      minitemp = temp;
      int minitemp2 = minitemp * 10;
      byte minitset1 = minitemp2 / 256;
      byte minitset2 = minitemp2 - minitset1 * 256;
      EEPROM.write(209, minitset1);  // partea intreaga
      EEPROM.write(210, minitset2);   // rest
    }
    if (temp > maxitemp) {
      maxitemp = temp;
      int maxitemp2 = maxitemp * 10;
      byte maxitset1 = maxitemp2 / 256;
      byte maxitset2 = maxitemp2 - maxitset1 * 256;
      EEPROM.write(211, maxitset1);  // partea intreaga
      EEPROM.write(212, maxitset2);   // rest
    }
    if (hum < minihum && hum > 0) {
      minihum = hum;
      EEPROM.write(213, minihum);
    }
    if (hum > maxihum) {
      maxihum = hum;
      EEPROM.write(214, maxihum);
    }
    //write on lcd
    temp = temp + faketemp;
    lcd.setCursor(0, 0);
    if (temp < 0)
    {
      t1 = -temp;
    }
    else t1 = temp;

    if ( t1 < 10)
    {
      lcd.print(" ");
    }
    if (temp > 0) lcd.print("+");
    if (temp == 0) lcd.print(" ");
    if (temp < 0) lcd.print("-");
    lcd.print(t1, 1);
    lcd.write(byte(0));
    lcd.print("C  ");

    hum = hum + fakehum;
    lcd.setCursor(10, 0);
    lcd.print(hum);
    lcd.print("%RH");


    if (reqtemp - temp > temphyst)
    {
      digitalWrite(heater, HIGH);
      digitalWrite(cooler, LOW);
      if(sound == 1){
      digitalWrite(buzzer, HIGH);
      }
      else digitalWrite(buzzer, LOW);
      lcd.setCursor(0, 1);
      lcd.print("COLD ");
    }

    else if (temp - reqtemp > temphyst)
    {

      digitalWrite(heater, LOW);
      digitalWrite(cooler, HIGH);
      if(sound == 1){
      digitalWrite(buzzer, HIGH);
      }
      else digitalWrite(buzzer, LOW);
      lcd.setCursor(0, 1);
      lcd.print("HOT   ");
    }

    else if (reqtemp - temp <= temphyst && temp - reqtemp <= temphyst)
    {
      digitalWrite(cooler, LOW);
      digitalWrite(heater, LOW);
      lcd.setCursor(0, 1);
      lcd.print("OK !  ");
    }
    else
    {
      lcd.setCursor(0, 1);
      lcd.print("OK !  ");
    }

    if (reqhum - hum > humhyst)
    {
      digitalWrite(wetter, HIGH);
      digitalWrite(dryer, LOW);
      if(sound == 1){
      digitalWrite(buzzer, HIGH);
      }
      else digitalWrite(buzzer, LOW);
      lcd.setCursor(10, 1);
      lcd.print("DRY ");
    }

    else if (hum - reqhum > humhyst)
    {
      digitalWrite(wetter, LOW);
      digitalWrite(dryer, HIGH);
      if(sound == 1){
      digitalWrite(buzzer, HIGH);
      }
      else digitalWrite(buzzer, LOW);
      lcd.setCursor(10, 1);
      lcd.print("WET  ");
    }

    else if (reqhum - hum <= humhyst && hum - reqhum <= humhyst)
    {
      digitalWrite(dryer, LOW);
      digitalWrite(wetter, LOW);
      lcd.setCursor(10, 1);
      lcd.print("OK !  ");
    }
    else
    {
      lcd.setCursor(10, 1);
      lcd.print("OK !  ");
    }
    if (reqhum - hum <= humhyst && hum - reqhum <= humhyst && reqtemp - temp <= temphyst && temp - reqtemp <= temphyst)
    {
      digitalWrite(buzzer, LOW);
      }
    /*

      if (reqtemp - temphyst <= temp)
      {
      if (heat == 1)
      {
      digitalWrite(heater, LOW);
      digitalWrite(cooler,HIGH);
      }
      if (heat == 0)
      {
      digitalWrite(heater, HIGH);
      digitalWrite(cooler, LOW);
      }

       lcd.setCursor(0, 1);
      //   lcd.print("OVER ");
      lcd.print("H0T   ");
      }

      if ((temp <= reqtemp) and (reqtemp - temphyst <= temp))
      {
       lcd.setCursor(0, 1);
       lcd.print("OK !  ");
      if (heat == 0) digitalWrite(heater, LOW);
      if (heat == 1) digitalWrite(heater, LOW);
      if (heat == 0) digitalWrite(cooler, LOW);
      if (heat == 1) digitalWrite(cooler, LOW);

      }



      if (reqtemp - temphyst > temp)
      {
      if (heat == 0) digitalWrite(heater, LOW);
      if (heat == 1) digitalWrite(heater, HIGH);
      if (heat == 0) digitalWrite(cooler, HIGH);
      if (heat == 1) digitalWrite(cooler, LOW);


       lcd.setCursor(0, 1);
      //   lcd.print("BELOW");
          lcd.print("COLD ");
      }
      if ((temp <= reqtemp) and (reqtemp - temphyst <= temp))
      {
       lcd.setCursor(0, 1);
       lcd.print("OK !  ");
      if (heat == 0) digitalWrite(heater, LOW);
      if (heat == 1) digitalWrite(heater, LOW);
      if (heat == 0) digitalWrite(cooler, LOW);
      if (heat == 1) digitalWrite(cooler, LOW);

      }

      if (hum > reqhum)
      {
      if (dry == 1) digitalWrite(wetter, HIGH);
      if (dry == 0) digitalWrite(wetter, LOW);
      if (heat == 1) digitalWrite(dryer, HIGH);
      if (heat == 0) digitalWrite(dryer, LOW);

       lcd.setCursor(10, 1);
      //   lcd.print("OVER ");
          lcd.print("WET ");
      }
      if (hum < reqhum - humhyst)
      {
      if (dry == 0) digitalWrite(wetter, HIGH);
      if (dry == 1) digitalWrite(wetter, LOW);
      if (heat == 0) digitalWrite(dryer, HIGH);
      if (heat == 1) digitalWrite(dryer, LOW);

       lcd.setCursor(10, 1);
      //   lcd.print("BELOW");
       lcd.print("DRY ");
      }
      if ((hum <= reqhum) and (hum >= reqhum - humhyst))
      {  lcd.setCursor(10, 1);
       lcd.print("OK !  ");
      if (dry == 0) digitalWrite(wetter, LOW);
      if (dry == 1) digitalWrite(wetter, LOW);
      if (heat == 0) digitalWrite(dryer, LOW);
      if (heat == 1) digitalWrite(dryer, LOW);
      if (heat == 0) digitalWrite(heater, LOW);
      if (heat == 1) digitalWrite(heater, LOW);
      }
    */
  } // last line for MENIU = 0


  if (meniu == 1) {
    while (meniu == 1) {
      //   reqtempet(tset);
      lcd.setCursor(0, 0);
      lcd.print("Temperature SET:");
      lcd.setCursor(0, 1);
      //   lcd.print("+");
      lcd.print(reqtemp, 1);
      //   lcd.write(0b11011111);
      lcd.write(byte(0));
      lcd.print("C  ");

      lcd.setCursor(8, 1);
      lcd.print("dt=");
      lcd.print(temphyst, 1);
      //   lcd.write(0b11011111);
      lcd.write(byte(0));
      lcd.print("C  ");


      if (digitalRead(BUT1) == LOW && reqtemp > 0)
      { reqtemp = reqtemp - 0.1;
        delay(250);
      }
      if (digitalRead(BUT2) == LOW && reqtemp < 99)
      { reqtemp = reqtemp + 0.1;
        delay(250);
      }

      int reqtemp2 = reqtemp * 10;
      byte tset1 = reqtemp2 / 256;
      byte tset2 = reqtemp2 - tset1 * 256;
      EEPROM.write(201, tset1);  // partea intreaga
      EEPROM.write(202, tset2);   // rest


      if (digitalRead(BUT3) == LOW)
      { meniu = 2;
        delay(250);
        lcd.clear();
      }
      //  delay(15);
    }
    delay (100);
  }   // end loop for MENIU = 1


  if (meniu == 2) {
    // if (tmax <= tmin) tmax = tmin + 10;
    while (meniu == 2) {
      //    maxim(dt/10); // this is number to diplay
      //    temphyst(dt); // this is number to diplay
      lcd.setCursor(0, 0);
      lcd.print("hyst.temp. SET:");
      lcd.setCursor(0, 1);
      //   lcd.print("+");
      lcd.print(reqtemp, 1);
      //   lcd.write(0b11011111);
      lcd.write(byte(0));
      lcd.print("C  ");

      lcd.setCursor(8, 1);
      lcd.print("dt=");
      lcd.print(temphyst, 1);
      //   lcd.write(0b11011111);
      lcd.write(byte(0));
      lcd.print("C  ");

      if (digitalRead(BUT1) == LOW )
      { temphyst = temphyst - 0.1;
        delay(250);
      }
      if (digitalRead(BUT2) == LOW)
      { temphyst = temphyst + 0.1;
        delay(250);
      }
      if (digitalRead(BUT3) == LOW)
      { meniu = 3;
        delay(250);
        lcd.clear();
      }
      //   delay(15);
      if (temphyst < 0.1) temphyst = 0.1;
      EEPROM.write(203, temphyst * 10);
    }
  }  // end loop for MENIU = 2


  if (meniu == 3) {
    while (meniu == 3) {
      //   reqtempet(tset);
      lcd.setCursor(0, 0);
      lcd.print("Humidity SET:");
      lcd.setCursor(0, 1);
      //   lcd.print("+");
      lcd.print(reqhum, 1);
      //   lcd.write(0b11011111);
      //   lcd.write(byte(0));
      lcd.print("%RH  ");

      lcd.setCursor(7, 1);
      lcd.print("dh=");
      lcd.print(humhyst);
      //   lcd.write(0b11011111);
      //   lcd.write(byte(0));
      lcd.print("%RH");


      if (digitalRead(BUT1) == LOW && reqhum > 0)
      { reqhum = reqhum - 1;
        delay(250);
      }
      if (digitalRead(BUT2) == LOW && reqhum < 100)
      { reqhum = reqhum + 1;
        delay(250);
      }

      EEPROM.write(204, reqhum);

      if (digitalRead(BUT3) == LOW)
      { meniu = 4;
        delay(250);
        lcd.clear();
      }
      //  delay(15);
    }
    delay (250);
  }  // end loop for MENIU =3


  if (meniu == 4) {
    while (meniu == 4) {
      lcd.setCursor(0, 0);
      lcd.print("hyst.humid. SET:");
      lcd.setCursor(0, 1);
      //   lcd.print("+");
      lcd.print(reqhum);
      //   lcd.write(0b11011111);
      lcd.write(byte(0));
      lcd.print("%RH");

      lcd.setCursor(7, 1);
      lcd.print("dh=");
      lcd.print(humhyst);
      //   lcd.write(0b11011111);
      lcd.write(byte(0));
      lcd.print("%RH");


      if (digitalRead(BUT1) == LOW)
      { humhyst = humhyst - 1;
        delay(250);
      }
      if (digitalRead(BUT2) == LOW)
      { humhyst = humhyst + 1;
        delay(250);
      }

      EEPROM.write(205, humhyst);

      if (digitalRead(BUT3) == LOW)
      { meniu = 5;
        delay(250);
        lcd.clear();
      }
      //   delay(15);
      if (humhyst < 1) humhyst = 1;
    }
  }  // end loop for MENIU = 4


  if (meniu == 5) {
    while (meniu == 5) {
      lcd.setCursor(0, 0);
      lcd.print("Fake Temprature");
      lcd.setCursor(0, 1);

      temp = dht.readTemperature();
      lcd.print("C=");
      if (temp > 0) lcd.print("+");
      if (temp == 0) lcd.print(" ");
      if (temp < 0) lcd.print("-");
      lcd.print(temp, 1);
      lcd.write(byte(0));
      lcd.print("C");
      lcd.setCursor(9, 1);
      if (faketemp > 0) lcd.print("+");
      if (faketemp == 0) lcd.print(" ");
      lcd.print(faketemp, 1);
      lcd.write(byte(0));
      lcd.print("C");

      if (digitalRead(BUT1) == LOW && faketemp > -50)
      { faketemp -= 0.1;
        delay(250);
      }
      if (digitalRead(BUT2) == LOW && faketemp < 100)
      { faketemp += 0.1;
        delay(250);
      }


      //      if (heat == 1)
      //      {
      //        lcd.print("HEATER ");
      //      }
      //      if (heat == 0)
      //      {
      //        lcd.print("COOLER ");
      //      }
      //
      //      if (digitalRead(BUT1) == LOW)
      //      { heat = 0;
      //        delay(250);
      //      }
      //      if (digitalRead(BUT2) == LOW)
      //      { heat = 1;
      //        delay(250);
      //      }
      //
      //      EEPROM.write(206, heat);

      if (digitalRead(BUT3) == LOW)
      { meniu = 6;
        delay(250);
        lcd.clear();
      }
    }
  }   // end loop for end MENIU = 5


  if (meniu == 6) {
    while (meniu == 6) {
      lcd.setCursor(0, 0);
      lcd.print("Fake Humidity");
      lcd.setCursor(0, 1);
      hum = dht.readHumidity();
      lcd.print("C=");
      lcd.print(hum);
      lcd.print("%RH");
      if (fakehum > 0) {
        lcd.setCursor(10, 1);
        lcd.print("+");
        lcd.print(fakehum);
        lcd.print("%RH");
      }
      else {
        lcd.setCursor(10, 1);
        lcd.print(fakehum);
        lcd.print("%RH");
      }

      if (digitalRead(BUT1) == LOW && fakehum > -100)
      { fakehum -= 1;
        delay(250);
      }
      if (digitalRead(BUT2) == LOW && faketemp < 100)
      { fakehum += 1;
        delay(250);
      }
      //if (dry == 1)
      //{
      //   lcd.print("DRYER ");
      //}
      //if (dry == 0)
      //{
      //   lcd.print("WETTER ");
      //}
      //
      //   if (digitalRead(BUT1) == LOW)
      //   { dry = 0;
      //   delay(250);
      //  }
      //   if (digitalRead(BUT2) == LOW)
      //   { dry = 1;
      //   delay(250);
      //  }
      //  EEPROM.write(207,dry);

      if (digitalRead(BUT3) == LOW)
      { meniu = 7;
        delay(250);
        lcd.clear();
      }
    }
  }// end loop for end MENIU = 6

   if (meniu == 7) {
    while (meniu == 7) {
      lcd.setCursor(0, 0);
      lcd.print("BUZZER");
      lcd.setCursor(0, 1);
      if(sound == 1)lcd.print("ON");
      else lcd.print("OFF");
      if (digitalRead(BUT1) == LOW || digitalRead(BUT2) == LOW) {
        sound=!sound;
        delay(250);
        lcd.clear();
      }
      if (digitalRead(BUT3) == LOW)
      { meniu = 8;
        delay(250);
        lcd.clear();
      }
    }
  }
  
  if (meniu == 8) {
    while (meniu == 8) {
      lcd.setCursor(0, 0);
      lcd.print("RESET?");
      lcd.setCursor(0, 1);
      if (digitalRead(BUT1) == LOW || digitalRead(BUT2) == LOW) {
        a = 0;
        lcd.clear();
        lcd.print("resetting");
        delay(1000);
        resetFunc();
      }
      if (digitalRead(BUT3) == LOW)
      { meniu = 9;
        delay(250);
        lcd.clear();
      }
    }
  }

  while (digitalRead(BUT1) == LOW) {
    buttonTimer += 1;
    //lcd.print(buttonTimer);
    delay(10);
  }
  if ((buttonTimer > 1) && (buttonTimer < longPressTime) )
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("minimum temp");
    lcd.setCursor(0, 1);
    if (minitemp > 0) lcd.print("+");
    if (minitemp == 0) lcd.print(" ");
    if (minitemp < 0) lcd.print("-");
    lcd.print(minitemp);
    lcd.write(byte(0));
    lcd.print("C  ");
    buttonTimer = 0;
    delay(3000);
    lcd.clear();
  }
  else if (buttonTimer > longPressTime) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("minimum humidity");
    lcd.setCursor(0, 1);
    lcd.print(minihum);
    lcd.print("%RH");
    buttonTimer = 0;
    delay(3000);
    lcd.clear();
  }

  while (digitalRead(BUT2) == LOW) {
    buttonTimer1 += 1;
    //  lcd.print(buttonTimer);
    delay(10);
  }
  if ((buttonTimer1 > 1) && (buttonTimer1 < longPressTime) )
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("maximum temp");
    lcd.setCursor(0, 1);
    if (maxitemp > 0) lcd.print("+");
    if (maxitemp == 0) lcd.print(" ");
    if (maxitemp < 0) lcd.print("-");
    lcd.print(maxitemp);
    lcd.write(byte(0));
    lcd.print("C  ");
    buttonTimer1 = 0;
    delay(3000);
    lcd.clear();
  }
  else if (buttonTimer1 > longPressTime) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("maximum humidity");
    lcd.setCursor(0, 1);
    lcd.print(maxihum);
    lcd.print("%RH");
    buttonTimer1 = 0;
    delay(3000);
    lcd.clear();
  }

  //this resets the arduino after 24 hours
  if (a >= 86400000) {
    a = 0;
    lcd.clear();
    lcd.print("resetting");
    delay(1500);
    resetFunc();
  }

}
