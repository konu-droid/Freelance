#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <HX711.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;
volatile double newencoderValue = 0;
volatile double offset = 0;

long lastencoderValue = 0;

float wei;
int16_t adc0 = 0;

int lastMSB = 0;
int lastLSB = 0;

const int rs = 9;
const int en = 8;
const int d4 = 7;
const int d5 = 6;
const int d6 = 5;
const int d7 = 4;
const int mode_switch = 1;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int out = A0;
const int clck = A1;
const int Tare = A2;
HX711 scale(out, clck);
float CalibrationFactor = -17230 ;  // Replace -12000 the calibration factor.

File logger;

#define time_button A3

/*
   for arduino uno
  SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10 (for MKRZero SD: SDCARD_SS_PIN)

   for arduino mega
  SD card attached to SPI bus as follows:
 ** MOSI - pin 51
 ** MISO - pin 50
 ** CLK - pin 52
 ** CS - pin 53 (for MKRZero SD: SDCARD_SS_PIN)
*/

// make the value 53 if u want to use it on arduino mega else 10 for uno
int pinCS = 10;

//edit this to increase datalogging speed in sd card
int interval = 1; //in seconds

int w = interval * 1; // 2 is previous value

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(time_button, INPUT_PULLUP);
  pinMode(mode_switch, INPUT_PULLUP);
  rtc.begin(); // Initialize the rtc object
  pinMode(Tare, INPUT);
  digitalWrite(Tare, HIGH);
  lcd.setCursor(0, 0);
  lcd.print("  Weight Scale");
  lcd.setCursor(0, 1);
  lcd.print("    Machine");

  delay(3000); // wait for console opening


  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);

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
  Serial.println("initialization done.");

  pinMode(pinCS, OUTPUT);
  DateTime now = rtc.now();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current Time:");
  lcd.setCursor(0, 1);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("Current Date:");
  lcd.setCursor(0, 1);
  lcd.print(now.year(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.print(" (");
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(") ");
  delay(2000);

  logger = SD.open("KN_log.txt", FILE_WRITE);
  if (logger) {

    logger.println("Date,Time,Weight,Displacement");
    logger.close(); // close the file
  }
  else {
    Serial.println("error opening KN_log.txt");
  }

  lcd.clear();
  scale.set_scale(CalibrationFactor);
  scale.tare();

  ads.begin();
}
void loop()
{
  DateTime now = rtc.now();
  if (digitalRead(time_button) == LOW) {
    delay(70);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Current Time:");
    lcd.setCursor(0, 1);
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    lcd.setCursor(0, 2);
    lcd.print("Current Date:");
    lcd.setCursor(0, 3);
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.print(" (");
    lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    lcd.print(") ");
    delay(3000);
    lcd.clear();
  }

  if (digitalRead(mode_switch) == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(scale.get_units(), 2);
    lcd.setCursor(13, 0);
    lcd.print(" KN");
    lcd.setCursor(0, 1);
    newencoderValue = (double)encoderValue;
    newencoderValue = (newencoderValue / 50) - offset;
    lcd.print(newencoderValue, 3);
    lcd.setCursor(13, 1);
    lcd.print(" mm");
    delay(500);
  }
  else {
    adc0 = ads.readADC_SingleEnded(0);
    adc0 = map(adc0, 0, 26666, 0, 5000);
    wei = adc0;
    wei = wei / 10;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(wei, 1);
    lcd.setCursor(13, 0);
    lcd.print(" KN");
    lcd.setCursor(0, 1);
    newencoderValue = (double)encoderValue;
    newencoderValue = (newencoderValue / 50) - offset;
    lcd.print(newencoderValue, 3);
    lcd.setCursor(13, 1);
    lcd.print(" mm");
    delay(500);
  }

  if (digitalRead(Tare) == LOW)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tare ......");
    lcd.setCursor(0, 1);
    lcd.print("Setting to mm 0.");
    offset = newencoderValue;
    delay(1000);
  }

  if (interval == w) {
    if (digitalRead(mode_switch) == LOW) {
      logger = SD.open("KN_log.txt", FILE_WRITE);
      if (logger) {
        logger.print(now.year(), DEC);
        logger.print('/');
        logger.print(now.month(), DEC);
        logger.print('/');
        logger.print(now.day(), DEC);
        logger.print(",");
        logger.print(now.hour(), DEC);
        logger.print(':');
        logger.print(now.minute(), DEC);
        logger.print(':');
        logger.print(now.second(), DEC);
        logger.print(",");
        logger.print(wei, 1);
        logger.print(",");
        logger.print(newencoderValue, 3);
        logger.print(",");
        logger.println("Mode 1");
        logger.close(); // close the file
      }
    }
    else if (digitalRead(mode_switch) == HIGH) {
      logger = SD.open("KN_log.txt", FILE_WRITE);
      if (logger) {
        logger.print(now.year(), DEC);
        logger.print('/');
        logger.print(now.month(), DEC);
        logger.print('/');
        logger.print(now.day(), DEC);
        logger.print(",");
        logger.print(now.hour(), DEC);
        logger.print(':');
        logger.print(now.minute(), DEC);
        logger.print(':');
        logger.print(now.second(), DEC);
        logger.print(",");
        logger.print(scale.get_units(), 2);
        logger.print(",");
        logger.print(newencoderValue, 3);
        logger.print(",");
        logger.println("Mode 2");
        logger.close(); // close the file
      }
    }
    // if the file didn't open, print an error:
    else {
      Serial.println("error opening KN_log.txt");
    }
    w = 0;
  }
  else if (w < interval) {
    w++;
  }
}

void updateEncoder() {
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}
