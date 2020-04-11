//tolerance works only in increment and once output high it works for 30 secs
//range 0.0 - 30.0

#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define A 2
#define B 3
#define C 4

// define variable
float volt1, volt2, volt3;
float req_volt1, req_volt2, req_volt3;
float volt1_hyst, volt2_hyst, volt3_hyst;
int count1 = 0;
int count2 = 0;
int count3 = 0;

//define flags
bool volt1_flag = 0;
bool volt2_flag = 0;
bool volt3_flag = 0;
bool volt1_sec_flag = 0;
bool volt2_sec_flag = 0;
bool volt3_sec_flag = 0;

#define POT1 A0
#define POT2 A1
#define POT3 A2
#define BUT1 5    // - switch
#define BUT2 6    // + switch
#define BUT3 7    // MENU switch

byte meniu = 0; // if MENIU = 0 is clasical
// if MENIU = 1 is for voltage set (req_volt)
// if MENIU = 2 is for dt voltage (volt_hyst)


void setup()
{
  Serial.begin(9600);

  lcd.begin(16, 2);

  // define outputs
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);

  // set the default state for outputs
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);

  // set push buttons for menu
  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);
  pinMode(POT3, INPUT);
  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);
  pinMode(BUT3, INPUT);

  // pull-ups on
  digitalWrite(BUT1, HIGH);
  digitalWrite(BUT2, HIGH);
  digitalWrite(BUT3, HIGH);

  lcd.setCursor(0, 1);
  lcd.print("Voltage control");

  delay(1000);
  lcd.clear();

  lcd.setCursor(0, 1);
  lcd.print("VER.0.1 GAGAN ");
  delay(1000);
  lcd.clear();

  byte tset1 = EEPROM.read(201);
  byte tset2 = EEPROM.read(202);
  req_volt1 = 256 * tset1 + tset2;  // recover the number
  req_volt1 = req_volt1 / 10;
  tset1 = EEPROM.read(203);
  tset2 = EEPROM.read(204);
  req_volt2 = 256 * tset1 + tset2;  // recover the number
  req_volt2 = req_volt2 / 10;
  tset1 = EEPROM.read(205);
  tset2 = EEPROM.read(206);
  req_volt3 = 256 * tset1 + tset2;  // recover the number
  req_volt3 = req_volt3 / 10;

  volt1_hyst = EEPROM.read(207);
  volt1_hyst = volt1_hyst / 10;
  volt2_hyst = EEPROM.read(208);
  volt2_hyst = volt2_hyst / 10;
  volt3_hyst = EEPROM.read(209);
  volt3_hyst = volt3_hyst / 10;
}

void loop()
{
  if (meniu >= 3) meniu = 0;
  if (digitalRead(BUT3) == LOW)
  { meniu = meniu + 1;
    lcd.clear();
    delay(250);
  }

  if (meniu == 0)
  {
    //get sensor values
    volt1 = analogRead(POT1);
    volt1 = map(volt1, 0, 1023, 0, 300);
    volt1 = volt1 / 10;
    volt2 = analogRead(POT2);
    volt2 = map(volt2, 0, 1023, 0, 300);
    volt2 = volt2 / 10;
    volt3 = analogRead(POT3);
    volt3 = map(volt3, 0, 1023, 0, 300);
    volt3 = volt3 / 10;

    //write on lcd
    lcd.setCursor(0, 0);
    lcd.print(volt1, 1);
    lcd.setCursor(5, 0);
    lcd.print(volt2, 1);
    lcd.setCursor(10, 0);
    lcd.print(volt3, 1);

    //checking conditions
    if ((req_volt1 + volt1_hyst > volt1 && volt1_flag == 1) || volt1_sec_flag == 1)
    {
      volt1_sec_flag = 1;
      digitalWrite(A, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("1");

      if (count1 < 3000) {
        delay(1);
        count1++;
      }
      else {
        volt1_sec_flag = 0;
        count1 = 0 ;
      }
    }

    else if (req_volt1 > volt1)
    {
      volt1_flag = 1;
    }

    else
    {
      digitalWrite(A, LOW);
      lcd.setCursor(0, 1);
      lcd.print("OK");
      volt1_flag = 0;
    }

    if ((req_volt2 + volt2_hyst > volt2 && volt2_flag == 1) || volt2_sec_flag == 1)
    {
      volt2_sec_flag = 1;
      digitalWrite(B, HIGH);
      lcd.setCursor(5, 1);
      lcd.print("2");

      if (count2 < 3000) {
        delay(1);
        count2++;
      }
      else {
        volt2_sec_flag = 0;
        count2 = 0 ;
      }
    }

    else if (req_volt2 > volt2)
    {
      digitalWrite(B, LOW);
      lcd.setCursor(5, 1);
      lcd.print("2");
      volt2_flag = 1;
    }

    else
    {
      digitalWrite(B, LOW);
      lcd.setCursor(5, 1);
      lcd.print("OK");
      volt2_flag = 0;
    }

    if ((req_volt3 + volt3_hyst > volt3 && volt3_flag == 1) || volt3_sec_flag == 1)
    {
      volt3_sec_flag = 1;
      digitalWrite(C, HIGH);
      lcd.setCursor(10, 1);
      lcd.print("3");

      if (count3 < 3000) {
        delay(1);
        count3++;
      }
      else {
        volt3_sec_flag = 0;
        count3 = 0 ;
      }
    }

    else if (req_volt3 > volt3)
    {
      digitalWrite(C, LOW);
      lcd.setCursor(10, 1);
      lcd.print("3");
      volt3_flag = 1;
    }

    else
    {
      digitalWrite(C, LOW);
      lcd.setCursor(10, 1);
      lcd.print("OK");
      volt3_flag = 0;
    }

  } // last line for MENIU = 0


  if (meniu == 1) {

    // Set voltage values
    while (meniu == 1) {
      while (digitalRead(BUT3) != LOW) {

        lcd.setCursor(0, 0);
        lcd.print("Voltage 1 SET: ");
        lcd.setCursor(0, 1);
        lcd.print(req_volt1, 1);


        if (digitalRead(BUT1) == LOW && req_volt1 > 0)
        { req_volt1 = req_volt1 - 0.1;
          delay(50);
        }
        if (digitalRead(BUT2) == LOW && req_volt1 < 30)
        { req_volt1 = req_volt1 + 0.1;
          delay(50);
        }
      }

      lcd.clear();
      delay(200);

      while (digitalRead(BUT3) != LOW) {

        lcd.setCursor(0, 0);
        lcd.print("Voltage 2 SET: ");
        lcd.setCursor(0, 1);
        lcd.print(req_volt2, 1);


        if (digitalRead(BUT1) == LOW && req_volt2 > 0)
        { req_volt2 = req_volt2 - 0.1;
          delay(50);
        }
        if (digitalRead(BUT2) == LOW && req_volt2 < 30)
        { req_volt2 = req_volt2 + 0.1;
          delay(50);
        }
      }

      lcd.clear();
      delay(200);

      while (digitalRead(BUT3) != LOW) {

        lcd.setCursor(0, 0);
        lcd.print("Voltage 3 SET: ");
        lcd.setCursor(0, 1);
        lcd.print(req_volt3, 1);


        if (digitalRead(BUT1) == LOW && req_volt3 > 0)
        { req_volt3 = req_volt3 - 0.1;
          delay(50);
        }
        if (digitalRead(BUT2) == LOW && req_volt3 < 30)
        { req_volt3 = req_volt3 + 0.1;
          delay(50);
        }
      }

      meniu = 2;
      delay(250);
      lcd.clear();

    }

    // saving values to eeprom
    int req2_volt1 = req_volt1 * 10;
    byte tset1 = req2_volt1 / 256;
    byte tset2 = req2_volt1 - tset1 * 256;
    EEPROM.write(201, tset1);
    EEPROM.write(202, tset2);
    delay (100);
    int req2_volt2 = req_volt2 * 10;
    tset1 = req2_volt2 / 256;
    tset2 = req2_volt2 - tset1 * 256;
    EEPROM.write(203, tset1);
    EEPROM.write(204, tset2);
    delay (100);
    int req2_volt3 = req_volt3 * 10;
    tset1 = req2_volt3 / 256;
    tset2 = req2_volt3 - tset1 * 256;
    EEPROM.write(205, tset1);
    EEPROM.write(206, tset2);
    delay (100);
  }   // end loop for MENIU = 1


  if (meniu == 2) {

    // set the tolerances
    while (meniu == 2) {

      while (digitalRead(BUT3) != LOW) {

        lcd.setCursor(0, 0);
        lcd.print("Hyst. Volt1 SET:");
        lcd.setCursor(0, 1);
        lcd.print(volt1_hyst, 1);

        if (digitalRead(BUT1) == LOW && volt1_hyst > 0.1 )
        { volt1_hyst = volt1_hyst - 0.1;
          delay(50);
        }
        if (digitalRead(BUT2) == LOW)
        { volt1_hyst = volt1_hyst + 0.1;
          delay(50);
        }
      }

      lcd.clear();
      delay(200);

      while (digitalRead(BUT3) != LOW) {

        lcd.setCursor(0, 0);
        lcd.print("Hyst. Volt2 SET:");
        lcd.setCursor(0, 1);
        lcd.print(volt2_hyst, 1);

        if (digitalRead(BUT1) == LOW && volt2_hyst > 0.1 )
        { volt2_hyst = volt2_hyst - 0.1;
          delay(50);
        }
        if (digitalRead(BUT2) == LOW)
        { volt2_hyst = volt2_hyst + 0.1;
          delay(50);
        }
      }

      lcd.clear();
      delay(200);

      while (digitalRead(BUT3) != LOW) {

        lcd.setCursor(0, 0);
        lcd.print("Hyst. Volt. SET:");
        lcd.setCursor(0, 1);
        lcd.print(volt3_hyst, 1);

        if (digitalRead(BUT1) == LOW && volt3_hyst > 0.1 )
        { volt3_hyst = volt3_hyst - 0.1;
          delay(50);
        }
        if (digitalRead(BUT2) == LOW)
        { volt3_hyst = volt3_hyst + 0.1;
          delay(50);
        }
      }

      meniu = 3;
      delay(350);
      lcd.clear();

    }
    EEPROM.write(207, volt1_hyst * 10);
    EEPROM.write(208, volt2_hyst * 10);
    EEPROM.write(209, volt3_hyst * 10);
  }  // end loop for MENIU = 2

}   // last line in main loop
