#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <HX711.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);


HX711 scale(A0, A1);    // parameter "gain" is ommited; the default value 128 is used by the library

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0, saveToSD = 0;
volatile long encoderValue = 0;
volatile double newencoderValue = 0, newencoderValueMAX = 0, lastnewencoderValue = 0, lastnewencoderValue2 = 0;
volatile double offset = 0;

volatile float kn = 0, max_kn = 0, lastkn = 0, lastkn2 = 0;
int16_t adc0 = 0;

int lastMSB = 0;
int lastLSB = 0;

const int otp = 4;
const int btn = 5;


const int out = A0;
const int clck = A1;
const int Tare = A2;
const int btn2 = 6;

float CalibrationFactor = -4380;  // Replace -12000 the calibration factor. AT 5 V 0.04 @ -50

File logger;

// make the value 53 if u want to use it on arduino mega else 10 for uno
int pinCS = 10;

//edit this to increase datalogging speed in sd card
int interval = 1; //in seconds
long timePassed = 0, timePassed2 = 0 ;
#define max_kn_button A3

float offset2 = 0;             // edit this for hx711
uint8_t action = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(max_kn_button, INPUT);
  digitalWrite(max_kn_button, HIGH);
  pinMode(btn, INPUT);
  digitalWrite(btn, HIGH);
  pinMode(otp, OUTPUT);
  digitalWrite(otp, LOW);

  rtc.begin(); // Initialize the rtc object
  pinMode(Tare, INPUT);
  digitalWrite(Tare, HIGH);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("++");
  lcd.setCursor(0, 1);
  lcd.print(" -- ");
  delay(1000); // wait for console opening
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" MADE BY ");
  lcd.setCursor(0, 1);
  lcd.print(" GAGAN");
  delay(100); // wait for console opening
  lcd.clear();


  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);



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
  delay(1000);
  pinMode(pinCS, OUTPUT);
  DateTime now = rtc.now();



  logger = SD.open("KN_log.txt", FILE_WRITE);
  if (logger) {

    logger.println("Date,Time,weight,last weight,max weight,Displacement,last Disp, max Disp");
    logger.close(); // close the file
  }
  else {
    Serial.println("error opening KN_log.txt");
  }

  lcd.clear();
  scale.set_scale(CalibrationFactor);
  // scale.tare();

  //    ads.begin();
}




void loop()
{
  DateTime now = rtc.now();
  newencoderValue = (double)encoderValue;
  newencoderValue = (newencoderValue / 100) - offset;

  kn = scale.get_units() - offset2;

  if (kn < 24000 ) kn = map(kn, 24000, 0, 0, 10);
  else kn = map(kn, 24000, 48000, 0, 10);
  kn = kn * 9.8;

  if (kn > max_kn)
    max_kn = kn;

  if (newencoderValue > newencoderValueMAX)
    newencoderValueMAX = newencoderValue;

  if (!digitalRead(btn)) {
    digitalWrite(otp, HIGH);
    action = 0;
  }

  if (digitalRead(otp) == HIGH) {
    if (action == 0 && kn > 0.5) {
      lastkn = 0;
      action = 1;
      timePassed2 = millis();
      saveToSD = 1;
    }
    else if ((action == 1 || action == 2) && kn > 0.5) {
      if (kn > lastkn) {
        lastkn = kn;
      }
      else if (lastkn - kn > 0.10) {
        lastkn2 = lastkn;
        action += 1;
      }
      if (newencoderValue > lastnewencoderValue) {
        lastnewencoderValue = newencoderValue;
      }
      else if (lastnewencoderValue - newencoderValue > 0.10) {
        lastnewencoderValue2 = lastnewencoderValue;
        action += 1;
      }
    }
    if (action != 0 && kn < 0.11) {
      digitalWrite(otp, LOW);
    }
  }

  lcd.clear();

  if (digitalRead(max_kn_button) == LOW) {
    lcd.setCursor(0, 0);
    lcd.print(max_kn, 2);
    lcd.setCursor(8, 0);
    lcd.print(" MAX KN");
    lcd.setCursor(0, 1);
    lcd.print(newencoderValueMAX, 2);
    lcd.setCursor(10, 1);
    lcd.print(" MAX mm");
  }

  else if (digitalRead(btn2) == LOW) {
    lcd.setCursor(0, 0);
    lcd.print(lastkn2, 2);
    lcd.setCursor(8, 0);
    lcd.print(" Last KN");
    lcd.setCursor(0, 1);
    lcd.print(lastnewencoderValue2, 2);
    lcd.setCursor(10, 1);
    lcd.print(" Last mm");
  }

  else {
    lcd.setCursor(0, 0);
    if (kn < 0) kn = 0;
    lcd.print(kn, 2);

    lcd.setCursor(10, 0);
    lcd.print("KN");
    lcd.setCursor(0, 1);
    lcd.print(newencoderValue, 3);
    lcd.setCursor(10, 1);
    lcd.print(" mm");
  }

  //void(* resetFunc) (void) = 0; //declare reset function @ address 0
  if (digitalRead(Tare) == LOW) {
    delay(150);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tare ......");
    lcd.setCursor(0, 1);
    lcd.print("KN & mm 0.");
    scale.tare();
    offset2 = 0;

    offset = newencoderValue;
    delay(1000);
  }

  if (millis() - timePassed >= interval * 1000) {
    if (digitalRead(otp) == HIGH && saveToSD == 1) {
      if (millis() - timePassed2 >= 15000) {
        saveToSD = 0;
      }
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
        logger.print(kn, 2);
        logger.print(",");
        logger.print(lastkn2, 2);
        logger.print(",");
        logger.print(max_kn, 2);
        logger.print(",");
        logger.print(newencoderValue, 2);
        logger.print(",");
        logger.print(lastnewencoderValue2, 2);
        logger.print(",");
        logger.println(newencoderValueMAX, 2);
        logger.close(); // close the file
      }
      else {
        Serial.println("error opening KN_log.txt");
      }
    }
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(",");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(",");
    Serial.print(kn, 2);
    Serial.print(",");
    Serial.print(lastkn2, 2);
    Serial.print(",");
    Serial.print(max_kn, 2);
    Serial.print(",");
    Serial.print(newencoderValue, 2);
    Serial.print(",");
    Serial.print(lastnewencoderValue2, 2);
    Serial.print(",");
    Serial.println(newencoderValueMAX, 2);
    timePassed = millis();



    // if the file didn't open, print an error:

  }

}

void updateEncoder() {
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bitS

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue--;

  lastEncoded = encoded; //store this value for next time
}
