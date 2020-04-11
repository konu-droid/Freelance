void calibration()
{
  float count = 0;
  while (digitalRead(Select) != LOW) {
    lcd.setCursor(0, 0);
    lcd.print("remove weights");
    lcd.setCursor(0, 1);
    lcd.print("and press ok!");
  }
  lcd.clear();
  tmr = scale.get_value(5);
  delay(1000);
  while (digitalRead(Select) != LOW) {
    lcd.setCursor(0, 0);
    lcd.print(count);
    if (digitalRead(up) == LOW) {
      count += 0.1;
      delay(250);
    }
    else if (digitalRead(down) == LOW) {
      count -= 0.1;
      delay(250);
    }
    lcd.setCursor(14, 0);
    lcd.print("KG");
    lcd.setCursor(0, 1);
    lcd.print("put known weight");
  }
  lcd.clear();
  long int pst_val;
  delay(250);
  pst_val = scale.get_value(5);
  pst_val = pst_val - tmr;
  count = count * 100;
  ten_grams = pst_val / count;
  max_show = (max_val / pst_val) * count * 1000;
  byte four = (ten_grams & 0xFF);
  byte three = ((ten_grams >> 8) & 0xFF);
  byte two = ((ten_grams >> 16) & 0xFF);
  byte one = ((ten_grams >> 24) & 0xFF);
  EEPROM.write(201, four);
  EEPROM.write(202, three);
  EEPROM.write(203, two);
  EEPROM.write(204, one);

  byte four1 = (tmr & 0xFF);
  byte three1 = ((tmr >> 8) & 0xFF);
  byte two1 = ((tmr >> 16) & 0xFF);
  byte one1 = ((tmr >> 24) & 0xFF);
  EEPROM.write(205, four1);
  EEPROM.write(206, three1);
  EEPROM.write(207, two1);
  EEPROM.write(208, one1);
}


void tmr_set() {
  tmr = scale.get_value(5);
  delay(250);
}
