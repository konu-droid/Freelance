#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <Stepper.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     // Use this for the 12-bit version 

//connect to GND
#define up A1
#define down A2
#define ok A3         //menu

//adjust this to change the amount of movement/ speed
const int stepsPerRevolution = 10;

//stepper pins
Stepper myStepper(stepsPerRevolution, 6, 7, 8, 9);

const int req_bars = 5;
int max_bars = 400;
float discount_factor;
int current_val = 0;
float current_val_float = 0;
int16_t max_adc = 65536;
float set_val = 0;

void setup() {
  //======stepper motor speed====
  myStepper.setSpeed(60);

  //==== ADC code ==
  //I2Cdev::begin();  // join I2C bus
  Serial.begin(9600);
  Serial.println("Hello!");

  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  ads.begin();

  //=======**======

  pinMode(up, INPUT);
  pinMode(down, INPUT);
  pinMode(ok, INPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Made by GAGAN");
  delay(2000);
}

void loop() {
  //=====================adc values===
  int16_t adc0;

  adc0 = ads.readADC_SingleEnded(0);
  //=====================**===========
  discount_factor = req_bars / max_bars;
  int16_t max_adc_val = max_adc / discount_factor;
  current_val = map(adc0, 0, max_adc_val, 0, 50000);
  current_val_float = current_val / 100;
  display_val();

  if (digitalRead(ok) == LOW) {
    lcd.clear();
    delay(500);
    while (digitalRead(ok) != LOW) {
      lcd.setCursor(0, 0);
      lcd.print("SET LIMIT");
      lcd.setCursor(0, 1);
      lcd.print(set_val);
      if (digitalRead(up) == LOW && set_val < 499.99) {
        delay(50);
        set_val += 0.01;
        lcd.clear();
      }
      if (digitalRead(down) == LOW && set_val > 0) {
        delay(50);
        set_val -= 0.01;
        lcd.clear();
      }
    }
    set_val = (set_val * 100) / 60;
    lcd.clear();
  }
  if (current_val > set_val)
  { 
    open_valve(1);
  }
  else if (current_val < set_val) {
     close_valve(1);
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("good pressure");
    display_val();
  }
}

void display_val() {
lcd.setCursor(0, 1);
  lcd.print("CUR = ");
  set_cursor(current_val_float,1);
  lcd.print(current_val_float, 2);
  lcd.setCursor(14, 1);
  lcd.print("KN");
  lcd.setCursor(0, 1);
  lcd.print("SET = ");
  set_cursor(set_val*(6/10),1);
  lcd.print(set_val*(6/10), 2);
  lcd.setCursor(14, 1);
  lcd.print("KN");
}

void open_valve(int sped) {
Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(10);
}

void close_valve(int sped) {
 Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(10);
}

void set_cursor(float check_val,int row){
  if(check_val>99){
    lcd.setCursor(9, row);
    }
    else if(check_val>9){
      lcd.setCursor(10, row);
      }
      else{
        lcd.setCursor(11, row);
        }
  }
