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

volatile float kn = 0, max_kn = 0, lastkn = 0, lastkn2 = 0, knM[13];

const int otp = 4;
const int btn = 5;// START KEY
const int otp2 = 7;//buzzer

const int out = A0;
const int clck = A1;
const int Tare = A2;
const int btn2 = 6;

float CalibrationFactor = -436 ;  // CALIBRATED-441,,, now -435

File logger;

// make the value 53 if u want to use it on arduino mega else 10 for uno
int pinCS = 10;

//edit this to increase datalogging speed in sd card
int interval = 0.02; //in seconds
long timePassed = 0, timePassed2 = 0 ;
#define max_kn_button A3

float offset2 = 117.8;             //113.4edit this for hx711
uint8_t action = 0, action1 = 0, action2 = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(otp2, OUTPUT);
  digitalWrite(otp2, LOW);
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
  lcd.print(" ok--- ");
  lcd.setCursor(0, 1);
  lcd.print(" t01 ");
  delay(1000); // wait for console opening
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" MADE BY ");
  lcd.setCursor(0, 1);
  lcd.print(" GAGAN");
  delay(200); // wait for console opening
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
    //   rtc.adjust(DateTime(2020, 5, 1, 13, 4, 0));
  }

  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }

  Serial.println("Initialization done.");
  delay(1000);
  pinMode(pinCS, OUTPUT);
  DateTime now = rtc.now();



  logger = SD.open("KN_log.txt", FILE_WRITE);
  if (logger) {

    logger.println("Date  ,Time  ,LOAD(N ),YIELD LOAD(N),BREAKING LOAD(N),Displacement(mm),YIELD Disp(mm),BREAKING Disp (mm)");
    logger.close(); // close the file
  }
  else {
    Serial.println("error opening KN_log.txt");
  }

  lcd.clear();
  scale.set_scale(CalibrationFactor);
  // scale.tare();

  //    ads.begin();
  for (int i = 0; i < 13; i++) {
    knM[i] = 0;
  }
}




void loop()
{
  DateTime now = rtc.now();
  newencoderValue = (double)encoderValue;
  newencoderValue = (newencoderValue / 36.10)  ; //17.4

  kn = scale.get_units(5) - offset2;  //taking average of 5 values
  
  //kn = map(kn, 24000,0,0,10);
  //kn = kn /9.8; // already set using calibration factor no need to mulip for nwetons 21 kg show 9.8 ok
  if (kn > max_kn)
    max_kn = kn;

  if (newencoderValue > newencoderValueMAX)
    newencoderValueMAX = newencoderValue;

  if (max_kn >= 6000 || newencoderValueMAX >= 650) digitalWrite(otp2, HIGH); // buzzer limit
  else {
    digitalWrite(otp2, LOW);
  }


  if (!digitalRead(btn)) {
    digitalWrite(otp, HIGH);

    action = 0;
  }



  if (digitalRead(otp) == HIGH) {
    if (action == 0 && (kn > 0.6)) {                   //                || newencoderValue > 0.05)) {
      lastkn = 0;
      action = 1;
      action2 = 0;
      action1 = 0;
      timePassed2 = millis();
      saveToSD = 1;
    }
    else if (action == 1 && (kn > 0.9)) {                 //          || newencoderValue > 0.05)) { // start load feature
      if (action1 == 0) {
        if (kn > lastkn) {
          lastkn = kn;
        }
        else if (lastkn -  kn > 0.50) {  // yeild point back
          lastkn2 = lastkn ;
          action1 = 1;
        }
      }
      if (action2 == 0) {
        if (newencoderValue > lastnewencoderValue) {
          lastnewencoderValue = newencoderValue;
        }
        else if (lastnewencoderValue - newencoderValue > 0.20) {
          lastnewencoderValue2 = lastnewencoderValue;
          action2 = 1;
        }
      }
    }

    if (action != 0 && kn < 0.20) {     // revese newton  auto off
      digitalWrite(otp, LOW);

    }

  }

  lcd.clear();

  if (digitalRead(max_kn_button) == LOW) {
    lcd.setCursor(0, 0);
    lcd.print("BREAK LOAD");
    lcd.setCursor(0, 2);
    lcd.print("LOAD (N)  =");
    lcd.setCursor(12, 2);
    lcd.print(max_kn, 2);
    lcd.setCursor(0, 3);
    lcd.print("Disp.(mm) =");
    lcd.setCursor(12, 3);
    lcd.print(newencoderValueMAX, 2);

  }

  else if (digitalRead(btn2) == LOW) {
    lcd.setCursor(0, 0);
    lcd.print("YIELD LOAD");
    lcd.setCursor(12, 2);
    lcd.print(lastkn2, 2);
    lcd.setCursor(0, 2);
    lcd.print("LOAD (N)  =");
    lcd.setCursor(12, 3);
    lcd.print(lastnewencoderValue2, 2);
    lcd.setCursor(0, 3);
    lcd.print("Disp.(mm) =");
  }

  else {
    lcd.setCursor(0, 0);
    if (kn < 0) kn = 0;  // minus hx711 plus value show

    lcd.setCursor(0, 0);
    lcd.print("LOAD  (N) =");
    lcd.setCursor(13, 0); // if (knAvg < 0) knAvg = 0;


    lcd.print(kn, 1);

    lcd.setCursor(0, 1);
    lcd.print("Disp.(mm) =");
    lcd.setCursor(13, 1);
    lcd.print(newencoderValue, 2);
  }

  //void(* resetFunc) (void) = 0; //declare reset function @ address 0
  if (digitalRead(Tare) == LOW) {
    delay(150);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tare ......");
    lcd.setCursor(0, 1);
    lcd.print("load & displacment..");
    scale.tare();
    offset2 = 0;

    offset = newencoderValue;
    delay(300);
  }

  if (millis() - timePassed >= interval * 1000) {
    if (digitalRead(otp) == HIGH && saveToSD == 1) {
      if (millis() - timePassed2 >= 350000) {
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