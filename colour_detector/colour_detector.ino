#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define S0 6
#define S1 7
#define S2 8
#define S3 9
#define sensorOut 10
int frequency = 0;
int red, green, blue, no_filt, level,pre_level=0;

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

  red = map(red, 255, 60, 0, 100);
  green = map(green, 255, 60, 0, 100);
  blue = map(blue, 175, 44, 0, 100); // 255,60.175,44 these are all sensor calibrated so chnage them when useing new sensor

  level = (red + green + blue) / 3;
  if(level < 0){
    level = 0;
    }
  if ((pre_level > 100 && 100 > level) || (pre_level > 10 && 10 > level)){
    lcd.clear();
    }
    lcd.setCursor(0, 0);
  lcd.print("WHITE LEVEL:");
  lcd.print(level);
  lcd.print("%");
  lcd.setCursor(0, 1);

  if (level == 0) {
    lcd.print("No Object");
  }
  else if ((red == green && red == blue) || (red - green < 3 && red - blue < 3 && red - green > -3 && red - blue > 3)) {
    lcd.print("white");
  }
  else if (red > green && red > blue) {
    lcd.print("red  ");
  }
  else if (red < green && green > blue) {
    lcd.print("green");
  }
  else if (blue > green && red < blue) {
    lcd.print("blue ");
  }
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
