void averaging() {
  delay(250);
  lcd.clear();
  int count = 2;
  delay(1000);
  while (digitalRead(Select) != LOW) {
    lcd.setCursor(0, 0);
    lcd.print("no. of samples?");
    if (digitalRead(up) == LOW) {
      delay(250);
      count++;
    }
    else if (digitalRead(down) == LOW) {
    delay(250);
      count--;
    }
    lcd.setCursor(0, 1);
    lcd.print(count);
  }
  avr = 0;
  for (int i = 1; i <= count; i++) {
    delay(250);
    lcd.clear();
    while (digitalRead(Select) != LOW) {
      top_line();
      lcd.setCursor(0, 1);
      lcd.print("sample");
      lcd.setCursor(14, 1);
      lcd.print(i);
    }
    avr += act_v;
  }
  delay(250);
  lcd.clear();
  while (digitalRead(Select) != LOW) {
    top_line();
    lcd.setCursor(0, 1);
    lcd.print("average ");
    lcd.setCursor(9, 1);
    lcd.print(avr/count);
    lcd.setCursor(14, 1);
    lcd.print("KG");
  }
}
