void change_unit()
{ delay(250);
lcd.clear();
  if (unit == 0) {
    while (digitalRead(Select) != LOW)
    {
      lcd.setCursor(0, 0);
      lcd.print("KG ==> NEWTON?");
    }
    lcd.clear();
  }
  else {
    lcd.clear();
    while (digitalRead(Select) != LOW)
    {
      lcd.setCursor(0, 0);
      lcd.print("NEWTON ==> KG?");
    }
    lcd.clear();
  }
  unit = !unit;
}
