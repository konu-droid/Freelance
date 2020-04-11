#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define CLK 40
#define CS 42

#define check_button 7

File logger;

/*
  SD card attached to SPI bus as follows:
 ** MOSI - pin 51
 ** MISO - pin 50
 ** CLK - pin 52
 ** CS - pin 53 (for MKRZero SD: SDCARD_SS_PIN)
*/

// connect CS pin of sd card module to pin 53 on mega
int pinCS = 53;

//connect sensor
// number      1 2 3 4 5 6 7 8
int DBIT[8] = {22, 24, 26, 28, 30, 32, 34, 36}; // SO pins of max3 ic

int v = 0;
float Ctemp[8] = {0, 0, 0, 0, 0, 0, 0, 0}, Ftemp[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// to calibrate temperature
// change each array element to calib each sensor in order
float sensor1_Calibrate = 0;
float sensor2_Calibrate = 0;
float sensor3_Calibrate = 0;
float sensor4_Calibrate = 0;
float sensor5_Calibrate = 0;
float sensor6_Calibrate = 0;
float sensor7_Calibrate = 0;
float sensor8_Calibrate = 0;


//edit this to increase datalogging speed in sd card
int interval = 2; //use multiples of 2 seconds

int w = 0;

float c_calib[8] = {
  sensor1_Calibrate,
  sensor2_Calibrate,
  sensor3_Calibrate,
  sensor4_Calibrate,
  sensor5_Calibrate,
  sensor6_Calibrate,
  sensor7_Calibrate,
  sensor8_Calibrate
};

void setup()   {
  Serial.begin(9600);
  pinMode(check_button, INPUT_PULLUP);
  rtc.begin(); // Initialize the rtc object
  lcd.begin(20, 4);

  delay(3000); // wait for console opening

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
  Serial.println("initialization done.");

  pinMode(pinCS, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DBIT[1], INPUT);
  pinMode(DBIT[2], INPUT);
  pinMode(DBIT[3], INPUT);
  pinMode(DBIT[4], INPUT);
  pinMode(DBIT[5], INPUT);
  pinMode(DBIT[6], INPUT);
  pinMode(DBIT[7], INPUT);
  pinMode(DBIT[0], INPUT);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  digitalWrite(CLK, LOW);

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Made by GAGAN");
  delay(2000);

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

  logger = SD.open("temp_log.txt", FILE_WRITE);
    if (logger) {
      logger.println("Date,Time,1,2,3,4,5,6,7,8,Average");
      logger.close(); // close the file
    }
    else {
      Serial.println("error opening temp_log.txt");
    }
    
  lcd.clear();
}

void loop()   {
  DateTime now = rtc.now();
  if (digitalRead(check_button) == LOW) {
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
  read_all_sen();

  lcd.setCursor(0, 0);
  lcd.print("1: ");
  lcd.print(Ctemp[0], 2);
  lcd.setCursor(9, 0);
  lcd.print("2: ");
  lcd.print(Ctemp[1], 2);
  lcd.setCursor(0, 1);
  lcd.print("3: ");
  lcd.print(Ctemp[2], 2);
  lcd.setCursor(9, 1);
  lcd.print("4: ");
  lcd.print(Ctemp[3], 2);
  lcd.setCursor(0, 2);
  lcd.print("5: ");
  lcd.print(Ctemp[4], 2);
  lcd.setCursor(9, 2);
  lcd.print("6: ");
  lcd.print(Ctemp[5], 2);
  lcd.setCursor(0, 3);
  lcd.print("7: ");
  lcd.print(Ctemp[6], 2);
  lcd.setCursor(9, 3);
  lcd.print("8: ");
  lcd.print(Ctemp[7], 2);


  if (interval == w) {
    logger = SD.open("temp_log.txt", FILE_WRITE);
    if (logger) {
      float sum = 0;
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
      for (int i = 0; i < 8; i++) {
        logger.print(",");
        logger.print(Ctemp[i]);
        sum += Ctemp[i];
      }
      sum = sum / 8;
      logger.print(",");
      logger.println(sum);
      logger.close(); // close the file
    }
    // if the file didn't open, print an error:
    else {
      Serial.println("error opening temp_log.txt");
    }
    w = 0;
  }
  else if (w < interval) {
    w++;
  }
}

void read_all_sen() {
  for (int i = 0; i < 8; i++) {
    v = spiRead(DBIT[i]);
    if (v == -1)   {
      Serial.print("No sensor \n");
    }
    else   {
      Ctemp[i] =  (v * 0.25) + c_calib[i];
      Ftemp[i] = (Ctemp[i] * 9 / 5) + 32;
      //Serial.println(Ftemp);
      // Serial.print("\n");
    }
  }
}

int spiRead(int senpin)   {
  int value = 0;
  digitalWrite(CS, LOW);
  delay(2);
  digitalWrite(CS, HIGH);
  delay(220);

  /* Read the chip and return the raw temperature value */
  /* Bring CS pin low to allow us to read the data from
    the conversion process */

  digitalWrite(CS, LOW);
  /* Cycle the clock for dummy bit 15 */
  digitalWrite(CLK, HIGH);
  delay(1);
  digitalWrite(CLK, LOW);

  /*
    Read bits 14-3 from MAX6675 for the Temp. Loop for each bit reading
    the value and storing the final value in 'temp'
  */

  for (int i = 14; i >= 0; i--) {
    digitalWrite(CLK, HIGH);
    value += digitalRead(senpin) << i;
    digitalWrite(CLK, LOW);
  }
  // check bit D2 if HIGH no sensor
  if ((value & 0x04) == 0x04) return -1;

  // shift right three places
  return value >> 3;
}
