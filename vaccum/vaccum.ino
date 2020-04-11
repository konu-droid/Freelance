#include <Wire.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte black[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

#define menu_button 6
#define exit_button 7
#define ok_button 8
#define run_button 9
#define up_button 10
#define down_button 13
#define sensor A0

int current_val = 0, old_current_val = 0;
int set_pressure[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int set_1holdtime[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int set_2holdtime = 0;
int hold_time = 300;
int password = 0001;

int gain = 0;

void setup() {
  lcd.begin(20, 4);
  lcd.createChar(0, black);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Made by GAGAN");
  delay(2000);

  pinMode(menu_button, INPUT_PULLUP);
  pinMode(exit_button, INPUT_PULLUP);
  pinMode(run_button, INPUT_PULLUP);
  pinMode(ok_button, INPUT_PULLUP);
  pinMode(up_button, INPUT_PULLUP);
  pinMode(down_button, INPUT_PULLUP);
  pinMode(sensor, INPUT);

  Serial.begin(57600);
  lcd.clear();
  lcd.setCursor(9, 1);
  lcd.print("SSS");
  lcd.setCursor(6, 3);
  lcd.print("Leak Test");
  delay(3000);
  lcd.clear();
}

void loop() {
  calculate_act_val();
  old_current_val = current_val;
  if (digitalRead(menu_button) == LOW) {
    delay(100);
    menu();
  }
  if (digitalRead(run_button) == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("Enter Method ID");
    int l = 1;
    int l_old = 0;
    while (digitalRead(ok_button) != LOW) {
      l_old = l;
      if (digitalRead(up_button) == LOW && l < 11 ) {
        delay(50);
        l = l + 1;
      }
      if (digitalRead(down_button) == LOW && l != 1) {
        delay(50);
        l = l - 1;
      }
      lcd_change_clear(l, l_old);
      lcd.println(l);
      lcd.println("Set Pressure   ");
      lcd.print(set_pressure[l]);
      lcd.println("1st Timmer   ");
      lcd.print(set_1holdtime[l]);
    }
    run_prg(l);
  }
}

void calculate_act_val() {
  current_val = analogRead(sensor);
  current_val = map(current_val, 0, 1023, 0, 760);
  lcd.setCursor(0, 0);
  lcd_change_clear(current_val, old_current_val);
  lcd.print("Current Val  ");
  lcd.print(current_val);
  lcd.print("NMHg");
}

void menu() {
  lcd.clear();
  int i = 0;
  while (digitalRead(exit_button) != LOW) {
    lcd.setCursor(0, 0);
    lcd.print("Vac_Pressure_&_Time,");
    lcd.setCursor(0, 1);
    lcd.print("Calibration, Method");
    lcd.setCursor(0, 2);
    lcd.print("Maintenance, Units");
    lcd.setCursor(0, 3);
    lcd.print("Date_&_Time, Result");
    if (digitalRead(up_button) == LOW && i < 7) {
      delay(100);
      i = i + 1;
    }
    if (digitalRead(down_button) == LOW && i != 0) {
      delay(100);
      i = i - 1;
    }
    switch (i) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.write(byte(0));
        break;

      case 1:
        lcd.setCursor(0, 1);
        lcd.write(byte(0));
        break;

      case 2:
        lcd.setCursor(13, 1);
        lcd.write(byte(0));
        break;

      case 3:
        lcd.setCursor(0, 2);
        lcd.write(byte(0));
        break;

      case 4:
        lcd.setCursor(13, 2);
        lcd.write(byte(0));
        break;

      case 5:
        lcd.setCursor(0, 3);
        lcd.write(byte(0));
        break;

      case 6:
        lcd.setCursor(13, 3);
        lcd.write(byte(0));
        break;
    }
    if (digitalRead(ok_button) == LOW) {
      switch (i) {
        case 0:
          lcd.clear();
          set_vaccum_func();
          break;

        case 1:
          lcd.clear();
          calib();
          break;

        case 2:
          lcd.clear();
          method();
          break;

        case 3:
          lcd.clear();
          maintenance();
          break;

        case 4:
          lcd.clear();
          units();
          break;

        case 5:
          lcd.clear();
          date_time();
          break;

        case 6:
          lcd.clear();
          result();
          break;
      }
    }
  }
}

void set_vaccum_func() {
  int j = 0;
  int j_old = 0;
  while (digitalRead(ok_button) != LOW) {
    lcd.setCursor(0, 0);
    lcd.println("Set Pressure");
    lcd.println("Range: 000 - 760");
    lcd.print("Value: ");
    lcd.print(j);
    j_old = j;
    if (digitalRead(up_button) == LOW && j < 760) {
      delay(50);
      j = j + 1;
    }
    if (digitalRead(down_button) == LOW && j != 0) {
      delay(50);
      j = j - 1;
    }
    lcd_change_clear(j, j_old);
  }

  lcd.clear();
  int k = 60;
  int mint_old = 0;
  int seco_old = 0;
  int mint = 0;
  int seco = 0;
  while (digitalRead(ok_button) != LOW) {
    lcd.setCursor(0, 0);
    lcd.println("Set 1st Hold Timer");
    lcd.println("Range: 00:59 - 99:59");
    lcd.print("value: ");
    mint = k / 60;
    seco = k % 60;
    lcd.print(mint);
    lcd.print(":");
    lcd.print(seco);
    if (digitalRead(up_button) == LOW && k < 6000 ) {
      delay(50);
      k = k + 1;
    }
    if (digitalRead(down_button) == LOW && k != 59) {
      delay(50);
      k = k - 1;
    }
    mint_old = k / 60;
    seco_old = k % 60;
    lcd_change_clear(mint, mint_old);
    lcd_change_clear(seco, seco_old);
  }

  lcd.clear();
  int l = 1;
  int l_old = 0;
  while (digitalRead(ok_button) != LOW) {
    lcd.setCursor(0, 0);
    lcd.println("Set Method Number");
    lcd.println("Max 10 ID's Only");
    lcd.print("ID : ");
    l_old = l;
    if (digitalRead(up_button) == LOW && l < 11 ) {
      delay(50);
      l = l + 1;
    }
    if (digitalRead(down_button) == LOW && l != 1) {
      delay(50);
      l = l - 1;
    }
    lcd_change_clear(l, l_old);
    lcd.print(l);
  }
  set_pressure[l] = j;
  set_1holdtime[l] = k;
}

void calib() {
  //edit gain value

}

void method() {
  lcd.setCursor(0, 0);
  lcd.print("nothing yet");
  delay(3000);
}

void maintenance() {
  lcd.setCursor(0, 0);
  lcd.println("Password");
  int j = 0;
  while (digitalRead(ok_button) != LOW) {
    if (digitalRead(up_button) == LOW && j < 760) {
      delay(50);
      j = j + 1;
    }
    if (digitalRead(down_button) == LOW && j != 0) {
      delay(50);
      j = j - 1;
    }
    lcd.print(j);
  }
  if (j == password) {
    lcd.clear();
    int k = 60;
    int mint_old = 0;
    int seco_old = 0;
    int mint = 0;
    int seco = 0;
    while (digitalRead(ok_button) != LOW) {
      lcd.setCursor(0, 0);
      lcd.println("Set 2st Hold Timer");
      lcd.println("Range: 00:59 - 99:59");
      lcd.print("value: ");
      mint = k / 60;
      seco = k % 60;
      lcd.print(mint);
      lcd.print(":");
      lcd.print(seco);
      if (digitalRead(up_button) == LOW && k < 6000 ) {
        delay(50);
        k = k + 1;
      }
      if (digitalRead(down_button) == LOW && k != 59) {
        delay(50);
        k = k - 1;
      }
      mint_old = k / 60;
      seco_old = k % 60;
      lcd_change_clear(mint, mint_old);
      lcd_change_clear(seco, seco_old);
    }
    set_2holdtime = k;
  }
  else {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("WRONG PASSWORD");
  }
}

void units() {
  lcd.setCursor(0, 0);
  lcd.print("nothing yet");
  delay(3000);
}

void date_time() {
  lcd.setCursor(0, 0);
  lcd.print("nothing yet");
  delay(3000);
}

void result() {

}

void run_prg(int val2) {
  calculate_act_val();
  int run_pressure = set_pressure[val2];
  int run_1hold = set_1holdtime[val2];
  
}

void lcd_change_clear(int z, int z_old) {
  if ((z == 9 && z_old == 10) || (z == 99 && z_old == 100))
  {
    lcd.clear();
  }
}
