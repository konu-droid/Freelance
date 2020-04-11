#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define S0 6
#define S1 7
#define S2 8
#define S3 9
#define sensorOut 10
#define pot A0

int frequency = 0;
int red, green, blue, no_filt, level,pre_level=0;

// change this to adjust the output
int calib = 0;

// change this for maximum range
int max_precentage = 100;

void setup() {
  lcd.begin(16, 2);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Made by GAGAN");
  delay(2000);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(pot, INPUT);

  // Setting frequency-scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  Serial.begin(9600);
  lcd.clear();
}

void loop() {
  readcolor();
  Serial.print("RGBN");
  Serial.print("  ");
  Serial.print(red);
  Serial.print("  ");
  Serial.print(green);
  Serial.print("  ");
  Serial.print(blue);
  Serial.print("  ");
  Serial.println(no_filt);

  red = map(red, 255, 60, 0, max_precentage);
  green = map(green, 255, 60, 0, max_precentage);
  blue = map(blue, 175, 44, 0, max_precentage);

  level = (red + green + blue) / 3;
  level = level-calib;
  
  if(level < 0){
    level = 0;
    }
  if(level > 100){
    level = 100;
    }
    
  if ((pre_level > 100 && 100 > level) || (pre_level > 10 && 10 > level)){
    lcd.clear();
    }
    lcd.setCursor(0, 0);
  lcd.print("WHITE LEVEL:");
  lcd.print(level);
  lcd.print("%");

pre_level = level;
}

void readcolor() {
  // red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  frequency = pulseIn(sensorOut, LOW);
  red = frequency;
  delay(100);

  // Green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  frequency = pulseIn(sensorOut, LOW);
  green = frequency;
  delay(100);

  // Blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  frequency = pulseIn(sensorOut, LOW);
  blue = frequency;
  delay(100);

  // no filter
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  frequency = pulseIn(sensorOut, LOW);
  no_filt = frequency;
  delay(100);

}
