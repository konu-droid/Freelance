#include <MPU6050_tockn.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

MPU6050 mpu6050(Wire);

float pitch, yaw, roll;
float diff = 0, diff2 = 0, diff3 = 0;

//#define ok 5
//#define right 7
//#define left 6

bool mode = 0;

//display variables
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

void setup() {
  /*
    delay(1000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.println("calibrating MPU6050 do not move");
    display.display();*/
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  DDRB |= _BV (0); // pinMode (8, OUTPUT);
  PORTB &= ~_BV (0); // digitalWrite (8, LOW);
  Serial.begin(38400);
  //DateTime now = rtc.now();
  //rtc.adjust(DateTime(2018, 11, 15, 12, 00, 0));

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  DDRD |= _BV (4); //pinMode(4, OUTPUT);
  PORTD |= _BV (4); // digitalWrite (4, HIGH);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

void loop() {
  get_Time();

  while (digitalRead(5) == LOW) {
    display.clearDisplay();
    display.display();
    delay(100);
    mpu6050.update();
    pitch = mpu6050.getAngleX();
    roll = mpu6050.getAngleY();
    yaw = mpu6050.getAngleZ();
    if (mode == 0) {
      while (digitalRead(5) == LOW) {
        scroll_and_click();
      }
    }
    else {
      while (digitalRead(5) == LOW) {
        cursor_move();
      }
    }
    display.clearDisplay();
    display.display();
  }
  if (digitalRead(7) == LOW && digitalRead(6) == LOW) {
    delay(150);
    mode = !mode;
    dis(10, 20);
    if (mode == 1) {
      display.println(F("SURF MODE"));
      display.display();
      delay(1500);
    }
    else {
      dis(10, 10);
      display.println(F("GESTURE"));
      display.setCursor(60, 21);
      display.println(F("MODE"));
      display.display();
      delay(1500);
    }
  }
  else if (digitalRead(7) == LOW) {
    delay(100);
    dis(10, 10);
    display.println(F("RIGHT"));
    display.setCursor(60, 21);
    display.println(F("CLICK"));
    display.display();
    Serial.write(254);
    delay(1000);
  }
  else if (digitalRead(6) == LOW) {
    delay(100);
    dis(10, 10);
    display.println(F("LEFT"));
    display.setCursor(60, 21);
    display.println(F("CLICK"));
    display.display();
    Serial.write(255);
    delay(1000);
  }
pitch = 0;
roll = 0;
yaw = 0;
}
