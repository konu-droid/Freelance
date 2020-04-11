void get_Time() {

  DateTime now = rtc.now();

  display.clearDisplay();

  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(5, 10);
  display.clearDisplay();
  display.print(now.hour(), DEC);
  display.print(":");
  display.print(now.minute(), DEC);

  display.setTextSize(1);
  display.setCursor(5, 45);
  display.print(daysOfTheWeek[now.dayOfTheWeek()]);

  display.setTextSize(1);
  display.setCursor(60, 55);
  display.print(now.day(), DEC);
  display.print('/');
  display.print(now.month(), DEC);
  display.print('/');
  display.print(now.year(), DEC);
  display.display();

  delay(1000);
  /*
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
  */
}
