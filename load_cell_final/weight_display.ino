void weight_display()
{
  weight();
  if (unit == 1) {
    act_v = act_v * 9.81;
    lcd.setCursor(0, 0);
    lcd.print(act_v);
    lcd.setCursor(9, 0);
    lcd.print("NEWTONS");
  }
  else if (unit == 0) {
    lcd.setCursor(0, 0);
    lcd.print(act_v);
    lcd.setCursor(14, 0);
    lcd.print("KG");
  }

}

void weight()
{
  long int raw = scale.read();
  raw -= tmr;
  act_v = raw / ten_grams;
  act_v = act_v / 100;

}

void fake_display() {
  weight();
  act_v = fake_measure * act_v ;
  if (unit == 1) {
    act_v = act_v * 9.81;
    lcd.setCursor(0, 0);
    lcd.print(act_v);
    lcd.setCursor(9, 0);
    lcd.print("NEWTONS");
  }
  else if (unit == 0) {
    lcd.setCursor(0, 0);
    lcd.print(act_v);
    lcd.setCursor(14, 0);
    lcd.print("KG");
  }
}

void top_line() {
  if (flag_limit == 1) {
    weight();
    if (act_v < mini && act_v < 0) {
      lcd.setCursor(0, 0);
      lcd.print("weight too low  ");
      delay(500);
      lcd.clear();
      delay(500);
    }
    else if (act_v > maxi) {
      lcd.setCursor(0, 0);
      lcd.print("weight too much ");
      delay(500);
      lcd.clear();
      delay(500);
    }
    else {
      if (flag_fake == 0 ) {
        weight_display();
      }
      else if (flag_fake == 1) {
        fake_display();
      }
    }
  }
  else {
    if (flag_fake == 0 ) {
      weight_display();
    }
    else if (flag_fake == 1) {
      fake_display();
    }
  }
}
