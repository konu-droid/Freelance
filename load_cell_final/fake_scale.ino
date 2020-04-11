void fake_scale()
{ delay(250);
  lcd.clear();
  float count = 0;
  while (digitalRead(Select) != LOW && digitalRead(tmr) != LOW ) {
    weight_display();
    lcd.setCursor(0, 1);
    lcd.print("fake ");
    lcd.setCursor(14, 1);
    lcd.print("KG");
    if (digitalRead(up) == LOW) {
      count += 0.1;
    }
    else if (digitalRead(down) == LOW) {
      count -= 0.1;
    }
    lcd.setCursor(9, 1);
    lcd.print(count);
    set_v = count;
  }
  if (digitalRead(tmr) == LOW) {
    delay(250);
    flag_fake = 0;
  }
  fake_measure = set_v / act_v;
}

