void scroll() {

  mpu6050.update();
  //diff =  pitch - mpu6050.getAngleX();
  diff2 = roll - mpu6050.getAngleY();
  //diff3 = yaw - mpu6050.getAngleZ();
  Serial.print(F("\tdiff : "));
  Serial.print(diff);
  Serial.print(F("\tdiff2 : "));
  Serial.print(diff2);
  Serial.print(F("\tdiff3 : "));
  Serial.println(diff3);
  if (diff2 > 15 ) {
    Serial.write(252);
    dis(10, 20);
    display.println(F("SCROLL UP"));
    display.display();
    delay(50);
  }
  else if (diff2 < -15) {
    Serial.write(253);
    dis(10, 10);
    display.println(F("SCROLL"));
    display.setCursor(60, 25);
    display.println(F("DOWN"));
    display.display();
    delay(50);
  }
}

void clicking() {

  mpu6050.update();
  //diff =  pitch - mpu6050.getAngleX();
  diff2 = roll - mpu6050.getAngleY();
  //diff3 = yaw - mpu6050.getAngleZ();
  Serial.print(F("\tdiff : "));
  Serial.print(diff);
  Serial.print(F("\tdiff2 : "));
  Serial.print(diff2);
  Serial.print(F("\tdiff3 : "));
  Serial.println(diff3);
  if (diff2 > 15 ) {
    Serial.write(254);
    dis(10, 20);
    display.println(F("SLIDE UP"));
    display.display();
    delay(1500);
  }
  else if (diff2 < -15) {
    Serial.write(255);
    dis(10, 10);
    display.println(F("SLIDE"));
    display.setCursor(60, 25);
    display.println(F("DOWN"));
    display.display();
    delay(1500);
  }
}

void cursor_move() {
  mpu6050.update();
  diff =  pitch - mpu6050.getAngleY();
  diff2 = roll - mpu6050.getAngleX();
  //diff3 = yaw - mpu6050.getAngleZ();

  if (diff2 > 25 && diff2 - diff > 0 ) {
    Serial.write(248);
    dis(10, 10);
    display.println(F("MOVING"));
    display.setCursor(30, 25);
    display.println(F("MOUSE Y"));
    display.display();
    delay(1);

  }
  else if (diff2 < -25 && diff2 - diff < 0) {
    Serial.write(249);
    dis(10, 10);
    display.println(F("MOVING"));
    display.setCursor(30, 25);
    display.println(F("MOUSE Y"));
    display.display();
    delay(1);

  }
  else if (diff > 25 && diff - diff2 > 0) {
    Serial.write(250);
    dis(10, 10);
    display.println(F("MOVING"));
    display.setCursor(30, 25);
    display.println(F("MOUSE X"));
    display.display();
    delay(1);

  }
  else if (diff < -25 && diff - diff2 < 0) {
    Serial.write(251);
    dis(10, 10);
    display.println(F("MOVING"));
    display.setCursor(30, 25);
    display.println(F("MOUSE X"));
    display.display();
    delay(1);

  }
}

void dis(int width, int height) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(width, height);
  display.clearDisplay();
}
