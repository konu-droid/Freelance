#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
 
void setup() { //initialise for at least 2s
  lcd.begin(16, 2);
  lcd.print("INITIALISING");
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.print(".");  
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.print(".");  
  delay(500);
}
void loop()
{
 int sensorValue = analogRead(A0);
 double dV = sensorValue;
 double le = (dV/1023)*100;
 int level = le;
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("LIGHT LEVEL:");
 lcd.print(level);
 lcd.print("%");
 lcd.setCursor(0, 1);
  
 if ((level >= 0) && (level <= 5))
 {
  lcd.print("VERY DARK"); 
 }
 else if ((level > 5) && (level <= 10))
 {
  lcd.print("DARK"); 
 }
 else if ((level > 10) && (level <= 50))
 {
  lcd.print("BRIGHT"); 
 }
 else
 {
  lcd.print("VERY BRIGHT"); 
 }
  
 delay(500); 
}
