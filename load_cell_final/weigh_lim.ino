void limit() {
  delay(250);
  lcd.clear();
  while (digitalRead(Select) != LOW) {
    lcd.setCursor(0, 0);
    lcd.print("minimum limit-KG");
    lcd.setCursor(14, 1);
    lcd.print("KG");
    if (digitalRead(up) == LOW) {
      delay(250);
      mini += 0.1;
    }
    else if (digitalRead(down) == LOW) {
      delay(250);
      mini -= 0.1;
    }
    lcd.setCursor(0, 1);
    lcd.print(mini);
  }
delay(250);
  while (digitalRead(Select) != LOW) {
    lcd.setCursor(0, 0);
    lcd.print("maximum limit-KG");
    lcd.setCursor(14, 1);
    lcd.print("KG");
    if (digitalRead(up) == LOW) {
      delay(250);
      maxi += 0.1;
    }
    else if (digitalRead(down) == LOW) {
      delay(250);
      maxi -= 0.1;
    }
    lcd.setCursor(0, 1);
    lcd.print(maxi);
  }
  lcd.clear();
}
