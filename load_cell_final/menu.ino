void menu_dis()
{ delay(250);
  lcd.clear();
  while (digitalRead(Select) != LOW && digitalRead(mode) != LOW) {
    lcd.setCursor(0, 0);
    lcd.print("CALIBRATION");
  }
  if (digitalRead(Select) == LOW)
  { delay(250);
    lcd.clear();
    calibration();
    delay(250);
  }
  else if (digitalRead(mode) == LOW) {
    delay(250);
    lcd.clear();
    while (digitalRead(Select) != LOW && digitalRead(mode) != LOW) {
      lcd.setCursor(0, 0);
      lcd.print("AVERAGE");
    }
    if (digitalRead(Select) == LOW)
    {
      delay(250);
      lcd.clear();
      averaging();
      delay(250);
    }
    else if (digitalRead(mode) == LOW) {
      delay(250);
      lcd.clear();

      while (digitalRead(Select) != LOW && digitalRead(mode) != LOW) {
        lcd.setCursor(0, 0);
        lcd.print("SET WEIGHT LIMIT");
      }
      if (digitalRead(Select) == LOW) {
        delay(250);
        lcd.clear();
        flag_limit = 1;
        limit();
        delay(250);
      }
      else if (digitalRead(mode) == LOW) {
        delay(250);
        lcd.clear();

        while (digitalRead(Select) != LOW && digitalRead(mode) != LOW) {

          lcd.setCursor(0, 0);
          lcd.print("CHANGE UNITS");
        }
        if (digitalRead(Select) == LOW) {
          delay(250);
          lcd.clear();
          change_unit();
          delay(250);
        }
        else if (digitalRead(mode) == LOW) {
          delay(250);
          lcd.clear();
          while (digitalRead(Select) != LOW && digitalRead(mode) != LOW) {

            lcd.setCursor(0, 0);
            lcd.print("FAKE SCALE ");
          }
          if (digitalRead(Select) == LOW) {
            delay(250);
            flag_fake = 1;
            fake_scale();
            delay(250);
          }
          lcd.clear();
        }
      }
    }
  }
}

