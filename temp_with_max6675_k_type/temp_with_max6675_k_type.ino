
/*    Max6675 Module  ==>   Arduino
      CS              ==>     D10
      SO              ==>     D12
      SCK             ==>     D13
      Vcc             ==>     Vcc (5v)
      Gnd             ==>     Gnd      */

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
int pinCS = 10;

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

#define MAX6675_CS   10
#define MAX6675_SO   12
#define MAX6675_SCK  13

// define pins for relay or leds
#define heater A0
#define cooler A1

// define variable
float temp, t1, reqtemp, temphyst;   // variable for temperature
int tempreached = 0, hstate = 0, cstate = 0, wstate = 0, dstate = 0, humreached = 0;
byte heat;  // is 1 for heater and 0 for cooler

//edit this to increase datalogging speed in sd card
int wait_time = 5;
int wait_count = 0;

/*
  #define BUT1 6    // - switch
  #define BUT2 7    // + switch
  #define BUT3 8    // MENU switch
*/

#define outputA 6
#define outputB 7
#define BUT1 8

int aState;
int aLastState;

byte meniu = 0; // if MENIU = 0 is clasical
// if MENIU = 1 is for temperature set (reqtemp)
// if MENIU = 2 is for dt temperature (temphyst)
// if MENIU = 3 is for HEAT/COOL


void setup()
{
  Serial.begin(9600);
  rtc.begin(); // Initialize the rtc object
  lcd.begin(20, 4);

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

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  DateTime now = rtc.now();
  Serial.println("initialization done.");

  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.createChar(0, grad);

  // define outputs
  pinMode(heater, OUTPUT);
  pinMode(cooler, OUTPUT);

  // set the default state for outputs
  digitalWrite(heater, LOW);
  digitalWrite(cooler, LOW);

  /*
    // set push buttons for menu
    pinMode(BUT1, INPUT);
    pinMode(BUT2, INPUT);
    pinMode(BUT3, INPUT);

    digitalWrite(BUT1, HIGH); // pull-ups on
    digitalWrite(BUT2, HIGH);
    digitalWrite(BUT3, HIGH);
  */

  pinMode (outputA, INPUT);
  pinMode (outputB, INPUT);
  pinMode(BUT1, INPUT);

  lcd.setCursor(0, 0);
  lcd.print("Thermostat");

  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("original sketch");

  lcd.setCursor(0, 1);
  lcd.print("ver.1.0 GAGAN ");
  delay(3000);
  lcd.clear();

  byte tset1 = EEPROM.read(201);
  byte tset2 = EEPROM.read(202);
  reqtemp = 256 * tset1 + tset2;  // recover the number
  reqtemp = reqtemp / 10;
  temphyst = EEPROM.read(203);
  temphyst = temphyst / 10;
  heat = EEPROM.read(206);

  templog = SD.open("templog.txt", FILE_WRITE);
  if (templog) {
    templog.println("Date,Time,Temp 1,Temp 2,Temp 3,Temp 4,Humidity 1,Humidity 2,Humidity 3,Humidity 4,Average Temp,Average Humidity,Set Values");
    templog.close(); // close the file
  }
  else {
    Serial.println("error opening templog.txt");
  }
  aLastState = digitalRead(outputA);

}

void loop()
{
  DateTime now = rtc.now();
  if (meniu >= 3) meniu = 0;
  if (digitalRead(BUT1) == LOW)
  { meniu = meniu + 1;
    lcd.clear();
    delay(250);
  }


  if (meniu == 0)
  {
    //get sensor values
    temp = readThermocouple();


    //write on lcd
    lcd.setCursor(0, 0);
    lcd.print("Temp ");
    lcd.setCursor(5, 0);

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


    if (reqtemp - temp > 0.5 && tempreached == 0)
    {
      if (hstate == 0)
      {
        digitalWrite(heater, HIGH);
        digitalWrite(cooler, LOW);
        hstate = 1;
        cstate = 0;
      }
      lcd.setCursor(12, 1);
      lcd.print("COLD");
    }

    else if (temp - reqtemp > 0.5 && tempreached == 0)
    {
      if (cstate == 0)
      {
        digitalWrite(heater, LOW);
        digitalWrite(cooler, HIGH);
        cstate = 1;
        hstate = 0;
      }
      lcd.setCursor(12, 1);
      lcd.print(" HOT");
    }

    else if (reqtemp - temp <= 0.5 && reqtemp - temp >= -0.5 && tempreached == 0)
    {
      tempreached = 1;
      digitalWrite(cooler, LOW);
      digitalWrite(heater, LOW);
      cstate = hstate = 0;
      lcd.setCursor(12, 1);
      lcd.print(" OK !");
    }

    else if (tempreached == 1 && (reqtemp - temp >= temphyst || temp - reqtemp >= temphyst))
    {
      tempreached = 0;

    }
    else
    {
      lcd.setCursor(12, 1);
      lcd.print(" OK !");
    }


    //Saving Temp values
    if (wait_time == wait_count) {
      templog = SD.open("log.txt", FILE_WRITE);
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
        templog.print(temp);
        templog.print(",");
        templog.print(reqtemp);
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

  } // last line for MENIU = 0


  if (meniu == 1) {
    while (meniu == 1) {
      aState = digitalRead(outputA);

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

      if (aState != aLastState) {

        if (digitalRead(outputB) != aState && reqtemp < 1024) {
          reqtemp = reqtemp + 0.1;
          delay(50);
        } else if (reqtemp > 0) {
          reqtemp = reqtemp - 0.1;
          delay(50);
        }
      }
      aLastState = aState;

      if (digitalRead(BUT1) == LOW)
      { meniu = 2;
        delay(250);
        lcd.clear();
      }
    }
    int reqtemp2 = reqtemp * 10;
    byte tset1 = reqtemp2 / 256;
    byte tset2 = reqtemp2 - tset1 * 256;
    EEPROM.write(201, tset1);  // partea intreaga
    EEPROM.write(202, tset2);   // rest
    delay (100);
  }   // end loop for MENIU = 1


  if (meniu == 2) {
    // if (tmax <= tmin) tmax = tmin + 10;
    while (meniu == 2) {
      aState = digitalRead(outputA);
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

      if (aState != aLastState) {

        if (digitalRead(outputB) != aState) {
          temphyst = temphyst + 0.1;
          delay(50);
        } else {
          temphyst = temphyst - 0.1;
          delay(50);
        }
      }
      aLastState = aState;

      if (digitalRead(BUT1) == LOW)
      { meniu = 3;
        delay(250);
        lcd.clear();
      }
      //   delay(15);
    }
    if (temphyst < 0.1) temphyst = 0.1;
    EEPROM.write(203, temphyst * 10);
  }  // end loop for MENIU = 2


  if (meniu == 3) {
    while (meniu == 5) {
      aState = digitalRead(outputA);
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

      if (aState != aLastState) {

        if (digitalRead(outputB) != aState) {
          heat = 0;
          delay(50);
        } else {
          heat = 1;
          delay(50);
        }
      }
      aLastState = aState;

      if (digitalRead(BUT1) == LOW)
      { meniu = 3;
        delay(250);
        lcd.clear();
      }
    }
    EEPROM.write(206, heat);
  }   // end loop for end MENIU = 3
}   // last line in main loop

double readThermocouple() {

  uint16_t v;
  pinMode(MAX6675_CS, OUTPUT);
  pinMode(MAX6675_SO, INPUT);
  pinMode(MAX6675_SCK, OUTPUT);

  digitalWrite(MAX6675_CS, LOW);
  delay(1);

  // Read in 16 bits,
  //  15    = 0 always
  //  14..2 = 0.25 degree counts MSB First
  //  2     = 1 if thermocouple is open circuit
  //  1..0  = uninteresting status

  v = shiftIn(MAX6675_SO, MAX6675_SCK, MSBFIRST);
  v <<= 8;
  v |= shiftIn(MAX6675_SO, MAX6675_SCK, MSBFIRST);

  digitalWrite(MAX6675_CS, HIGH);
  if (v & 0x4)
  {
    // Bit 2 indicates if the thermocouple is disconnected
    return NAN;
  }

  // The lower three bits (0,1,2) are discarded status bits
  v >>= 3;

  // The remaining bits are the number of 0.25 degree (C) counts
  return v * 0.25;
}
