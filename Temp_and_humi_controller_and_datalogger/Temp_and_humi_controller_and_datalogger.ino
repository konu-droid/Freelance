#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

File templog;

// connect CS pin of sd card module to pin 53 on mega
int pinCS = 53;

// http://arduino.cc/en/Reference/LiquidCrystalCreateChar
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
#define DHTPIN1 2     // what pin we're connected to 2
#define DHTPIN2 3     // what pin we're connected to 3
#define DHTPIN3 4     // what pin we're connected to 4
#define DHTPIN4 5     // what pin we're connected to 5

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// if is just sensor:0
// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
DHT dht3(DHTPIN3, DHTTYPE);
DHT dht4(DHTPIN4, DHTTYPE);

// define pins for relay or leds
#define heater A0
#define wetter A2
#define cooler A1
#define dryer A3

// define variable
float temp, t1, reqtemp, temphyst;   // variable for temperature
float hum, reqhum, humhyst;        // variable for humiditiy
float temp1, temp2, temp3, temp4;
float hum1, hum2, hum3, hum4;

byte heat;  // is 1 for heater and 0 for cooler
byte dry;   // is 1 for dryer and 0 for wetter

//edit this to increase datalogging speed in sd card
int wait_time = 60;
int wait_count = 0;

#define POT1 A6
#define POT2 A6
#define data_time_pin A7
#define BUT1 6    // - switch
#define BUT2 7    // + switch
#define BUT3 8    // MENU switch

byte meniu = 0; // if MENIU = 0 is clasical
// if MENIU = 1 is for temperature set (reqtemp)
// if MENIU = 2 is for dt temperature (temphyst)
// if MENIU = 3 is for humidity set (reqhum)
// if MENIU = 4 is for dh humidity (humhyst)
// if MENIU = 5 is for HEAT/COOL
// if MENIU = 6 id for DRY/WET

int total_time = 1500;    //in miliseconds


void setup()
{
  Serial.begin(9600);
  rtc.begin(); // Initialize the rtc object

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }
  DateTime now = rtc.now();
  Serial.println("initialization done.");

  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.begin(20, 4);
  lcd.createChar(0, grad);

  // Initialize DHT sensor
  dht1.begin();
  dht2.begin();
  dht3.begin();
  dht4.begin();

  // define outputs
  pinMode(heater, OUTPUT);
  pinMode(wetter, OUTPUT);
  pinMode(cooler, OUTPUT);
  pinMode(dryer, OUTPUT);

  // set the default state for outputs
  digitalWrite(heater, LOW);
  digitalWrite(wetter, LOW);
  digitalWrite(cooler, LOW);
  digitalWrite(dryer, LOW);

  // set push buttons for menu
  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);
  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);
  pinMode(BUT3, INPUT);
  pinMode(data_time_pin, INPUT);

  digitalWrite(BUT1, HIGH); // pull-ups on
  digitalWrite(BUT2, HIGH);
  digitalWrite(BUT3, HIGH);

  lcd.setCursor(0, 0);
  lcd.print("Temperature with");

  lcd.setCursor(0, 1);
  lcd.print("humidity controls");

  delay(1000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temp & Hum DH4");

  lcd.setCursor(0, 1);
  lcd.print("VER.0.1 GAGAN ");
  delay(1000);
  lcd.clear();

  byte tset1 = EEPROM.read(201);
  byte tset2 = EEPROM.read(202);
  reqtemp = 256 * tset1 + tset2;  // recover the number
  reqtemp = reqtemp / 10;
  temphyst = EEPROM.read(203);
  temphyst = temphyst / 10;
  tset1 = EEPROM.read(204);
  tset2 = EEPROM.read(208);
  reqhum = 256 * tset1 + tset2;  // recover the number
  reqhum = reqhum / 10;
  humhyst = EEPROM.read(205);
  humhyst = humhyst / 10;
  heat = EEPROM.read(206);
  dry = EEPROM.read(207);

  templog = SD.open("templog.txt", FILE_WRITE);
  if (templog) {
    templog.println("Date,Time,Temp 1,Temp 2,Temp 3,Temp 4,Humidity 1,Humidity 2,Humidity 3,Humidity 4,Average Temp,Average Humidity,Set Values");
    templog.close(); // close the file
  }
  else {
    Serial.println("error opening templog.txt");
  }

}

void loop()
{
  DateTime now = rtc.now();
  if (meniu >= 7) meniu = 0;
  if (digitalRead(BUT3) == LOW)
  { meniu = meniu + 1;
    lcd.clear();
    delay(250);
  }

  if (digitalRead(data_time_pin)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DATE :");
    lcd.setCursor(0, 1);
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.setCursor(0, 2);
    lcd.print("TIME :");
    lcd.setCursor(0, 3);
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    delay(2000);
    lcd.clear();
  }


  if (meniu == 0)
  {
    //get sensor values
    hum1 = dht1.readHumidity() - 5.5;
    hum2 = dht2.readHumidity() - 5.5; //-10
    hum3 = dht3.readHumidity() - 5.5;
    hum4 = dht4.readHumidity() - 5.5;
    temp1 = dht1.readTemperature() + 0.2; //+1
    temp2 = dht2.readTemperature() + 0.1; //-0.5
    temp3 = dht3.readTemperature() + 0.1;
    temp4 = dht4.readTemperature() + 0.4;

    temp = (temp1 + temp2 + temp3 + temp4) / 4;
    hum = ( hum1 + hum2 + hum3 + hum4 ) / 4;           //(hum1 + hum2 + hum3 + hum4)
    // delay(100);
    if (isnan(hum) || isnan(temp)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {

      //write on lcd
      lcd.setCursor(0, 0);
      lcd.print(temp1, 1);
      lcd.setCursor(5, 0);
      lcd.print(temp2, 1);
      lcd.setCursor(10, 0);
      lcd.print(temp3, 1);
      lcd.setCursor(15, 0);
      lcd.print(temp4, 1);

      lcd.setCursor(0, 1);
      lcd.print(hum1, 1);
      lcd.setCursor(5, 1);
      lcd.print(hum2, 1);
      lcd.setCursor(10, 1);
      lcd.print(hum3, 1);
      lcd.setCursor(15, 1);
      lcd.print(hum4, 1);

      lcd.setCursor(0, 2);
      lcd.print("Temp ");
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
      lcd.print(t1, 2);
      lcd.write(byte(0));
      lcd.print("C  ");

      lcd.setCursor(0, 3);
      lcd.print("Hum ");
      lcd.print(hum, 2);
      lcd.print("%RH");
      //  delay(900);

      if (reqtemp - temp > temphyst)
      {
        digitalWrite(heater, HIGH);
        digitalWrite(cooler, LOW);
        lcd.setCursor(16, 2);
        lcd.print("COLD");
      }

      else if (temp - reqtemp > temphyst)
      {
        digitalWrite(heater, LOW);
        digitalWrite(cooler, HIGH);
        lcd.setCursor(16, 2);
        lcd.print(" HOT");
      }

      else if (reqtemp - temp > 0 )
      {
        int ON_time = analogRead(POT1);
        lcd.setCursor(16, 2);
        lcd.print(" OK");
        digitalWrite(heater, HIGH);
        delay(ON_time);
        digitalWrite(heater, LOW);
        delay(total_time - ON_time);
      }

      else
      {
        digitalWrite(cooler, LOW);
        digitalWrite(heater, LOW);
        lcd.setCursor(16, 2);
        lcd.print(" OK !");
      }

      if (reqhum - hum > humhyst)
      {
        digitalWrite(wetter, HIGH);
        digitalWrite(dryer, LOW);
        lcd.setCursor(16, 3);
        lcd.print(" DRY");
      }

      else if (hum - reqhum > humhyst)
      {
        digitalWrite(wetter, LOW);
        digitalWrite(dryer, HIGH);
        lcd.setCursor(16, 3);
        lcd.print(" WET");
      }

      else if (reqhum - hum > 0)
      {
        int ON_time = analogRead(POT2);
        lcd.setCursor(16, 3);
        lcd.print(" OK");
        digitalWrite(wetter, HIGH);
        delay(ON_time);
        digitalWrite(wetter, LOW);
        delay(total_time - ON_time);
      }

      else
      {
        digitalWrite(dryer, LOW);
        digitalWrite(wetter, LOW);
        lcd.setCursor(16, 3);
        lcd.print(" OK !");
      }

      //Saving Temp values
      if (wait_time == wait_count) {
        templog = SD.open("templog.txt", FILE_WRITE);
        if (templog) {
          templog.print(now.year(), DEC);
          templog.print('/');
          templog.print(now.month(), DEC);
          templog.print('/');
          templog.print(now.day(), DEC);
          templog.print(",");
          templog.print(now.hour(), DEC);
          templog.print(':');
          templog.print(now.minute(), DEC);
          templog.print(':');
          templog.print(now.second(), DEC);
          templog.print(",");
          templog.print(temp1);
          templog.print(",");
          templog.print(temp2);
          templog.print(",");
          templog.print(temp3);
          templog.print(",");
          templog.print(temp4);
          templog.print(",");
          templog.print(hum1);
          templog.print(",");
          templog.print(hum2);
          templog.print(",");
          templog.print(hum3);
          templog.print(",");
          templog.print(hum4);
          templog.print(",");
          templog.print(temp);
          templog.print(",");
          templog.print(hum);
          templog.print(",");
          templog.print(reqtemp);
          templog.print(",");
          templog.println(reqhum);
          templog.close(); // close the file
        }

        // if the file didn't open, print an error:
        else {
          Serial.println("error opening log.txt");
        }
        wait_count = 0;
      }
      else if (wait_count < wait_time) {
        wait_count++;
      }
    }

  } // last line for MENIU = 0


  if (meniu == 1) {
    while (meniu == 1) {
      //   reqtempet(tset);
      lcd.setCursor(0, 0);
      lcd.print("Temperature SET: ");
      lcd.setCursor(0, 1);
      //   lcd.print("+");
      lcd.print(reqtemp, 1);
      //   lcd.write(0b11011111);
      lcd.write(byte(0));
      lcd.print("C  ");

      lcd.setCursor(0, 2);
      lcd.print("dt=");
      lcd.print(temphyst, 1);
      //   lcd.write(0b11011111);
      lcd.write(byte(0));
      lcd.print("C  ");


      if (digitalRead(BUT1) == LOW)
      { reqtemp = reqtemp - 0.1;
        delay(50);
      }
      if (digitalRead(BUT2) == LOW)
      { reqtemp = reqtemp + 0.1;
        delay(50);
      }


      if (digitalRead(BUT3) == LOW)
      { meniu = 2;
        delay(250);
        lcd.clear();
      }
    }
    int reqtemp2 = reqtemp * 10;
    byte tset1 = reqtemp2 / 256;
    byte tset2 = reqtemp2 - tset1 * 256;
    EEPROM.write(201, tset1);
    EEPROM.write(202, tset2);
    delay (100);
  }   // end loop for MENIU = 1


  if (meniu == 2) {
    // if (tmax <= tmin) tmax = tmin + 10;
    while (meniu == 2) {
      //    maxim(dt/10); // this is number to diplay
      //    temphyst(dt); // this is number to diplay
      lcd.setCursor(0, 0);
      lcd.print("Hyst. Temp. SET:");
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

      if (digitalRead(BUT1) == LOW && temphyst > 0.1 )
      { temphyst = temphyst - 0.1;
        delay(50);
      }
      if (digitalRead(BUT2) == LOW)
      { temphyst = temphyst + 0.1;
        delay(50);
      }
      if (digitalRead(BUT3) == LOW)
      { meniu = 3;
        delay(350);
        lcd.clear();
      }
    }
    EEPROM.write(203, temphyst * 10);
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
      lcd.print(" %RH ");

      lcd.setCursor(0, 2);
      lcd.print("dh=");
      lcd.print(humhyst, 1);
      //   lcd.write(0b11011111);
      //   lcd.write(byte(0));
      lcd.print(" %RH");


      if (digitalRead(BUT1) == LOW && reqhum > 0)
      { reqhum = reqhum - 0.1;
        delay(50);
      }
      if (digitalRead(BUT2) == LOW)
      { reqhum = reqhum + 0.1;
        delay(50);
      }

      if (digitalRead(BUT3) == LOW)
      { meniu = 4;
        delay(350);
        lcd.clear();
      }
      //  delay(15);
    }
    int reqtemp2 = reqhum * 10;
    byte tset1 = reqtemp2 / 256;
    byte tset2 = reqtemp2 - tset1 * 256;
    EEPROM.write(204, tset1);
    EEPROM.write(208, tset2);
    delay (350);
  }  // end loop for MENIU =3


  if (meniu == 4) {
    while (meniu == 4) {
      lcd.setCursor(0, 0);
      lcd.print("Hyst. Humid. SET:");
      lcd.setCursor(0, 1);
      //   lcd.print("+");
      lcd.print(reqhum, 1);
      //   lcd.write(0b11011111);
      lcd.write(byte(0));
      lcd.print(" %RH");

      lcd.setCursor(0, 2);
      lcd.print("dh=");
      lcd.print(humhyst, 1);
      //   lcd.write(0b11011111);
      lcd.write(byte(0));
      lcd.print(" %RH");


      if (digitalRead(BUT1) == LOW && humhyst > 0.1)
      { humhyst = humhyst - 0.1;
        delay(50);
      }
      if (digitalRead(BUT2) == LOW)
      { humhyst = humhyst + 0.1;
        delay(50);
      }

      if (digitalRead(BUT3) == LOW)
      { meniu = 5;
        delay(350);
        lcd.clear();
      }
    }
    EEPROM.write(205, humhyst * 10);
  }  // end loop for MENIU = 4


  if (meniu == 5) {
    while (meniu == 5) {
      lcd.setCursor(0, 0);
      lcd.print("thermostat type:");
      lcd.setCursor(0, 1);

      if (heat == 1)
      {
        lcd.print("HEATER ");
      }
      if (heat == 0)
      {
        lcd.print("COOLER ");
      }

      if (digitalRead(BUT1) == LOW)
      { heat = 0;
        delay(50);
      }
      if (digitalRead(BUT2) == LOW)
      { heat = 1;
        delay(50);
      }

      if (digitalRead(BUT3) == LOW)
      { meniu = 6;
        delay(350);
        lcd.clear();
      }
    }
    EEPROM.write(206, heat);
  }   // end loop for end MENIU = 5


  if (meniu == 6) {
    while (meniu == 6) {
      lcd.setCursor(0, 0);
      lcd.print("Humidity control");
      lcd.setCursor(0, 1);

      if (dry == 1)
      {
        lcd.print("DRYER ");
      }
      if (dry == 0)
      {
        lcd.print("WETTER ");
      }

      if (digitalRead(BUT1) == LOW)
      { dry = 0;
        delay(50);
      }
      if (digitalRead(BUT2) == LOW)
      { dry = 1;
        delay(50);
      }

      if (digitalRead(BUT3) == LOW)
      { meniu = 7;
        delay(250);
        lcd.clear();
      }
    }
    EEPROM.write(207, dry);
  }   // end loop for end MENIU = 6
}   // last line in main loop
