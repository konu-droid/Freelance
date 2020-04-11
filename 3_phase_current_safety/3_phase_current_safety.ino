#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//inputs
#define A A0
#define B A1
#define C A2

//termistor
const int Switch = 13;

//feedbacks
#define feedback_J A4
#define feedback_K A5
#define feedback_D 10

//outputs
#define E 6 //start
#define G 7 //stop
#define H 8
#define I 9

//variables
int a_sense, b_sense, c_sense, feedback_d, feedback_j, feedback_k;
bool a_sw = 0, b_sw = 0, c_sw = 0;

//constants
const int low_limit = 614;
const int high_limit = 820;

//states
bool state1 = 1;
bool state2 = 1;
bool on_off = 0;

void setup() {

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Made by GAGAN");
  delay(2000);
  pinMode(A, INPUT);
  pinMode(B, INPUT);
  pinMode(C, INPUT);
  pinMode(feedback_D, INPUT);
  pinMode(feedback_J, INPUT_PULLUP);
  pinMode(feedback_K, INPUT_PULLUP);
  pinMode(Switch, INPUT);

  pinMode(E, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(I, OUTPUT);
  pinMode(H, OUTPUT);

  Serial.begin(9600);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 1);

  a_sense = analogRead(A);
  b_sense = analogRead(B);
  c_sense = analogRead(C);
  feedback_d = digitalRead(feedback_D);

  if (digitalRead(Switch) == 1) {
    on_off = 1;
  }
  else if (digitalRead(Switch) == 0) {
    on_off = 0;
    lcd.setCursor(0, 0);
    lcd.print("Switch is off   ");
  }

  if (a_sense > low_limit && a_sense < high_limit) {
    a_sw = 0;
  }
  else {
    a_sw = 1;
  }
  if (b_sense > low_limit && b_sense < high_limit) {
    b_sw = 0;
  }
  else {
    b_sw = 1;
  }
  if (c_sense > low_limit && c_sense < high_limit) {
    c_sw = 0;
  }
  else {
    c_sw = 1;
  }

  //looping conditions
  if ((a_sw == 1 || b_sw == 1 || c_sw == 1) && on_off == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BAD VOLTAGE");
    delay(10000);
    int j = 0;
    feedback_d = digitalRead(feedback_D);
    while (feedback_d != HIGH && j < 5) {
      digitalWrite(E, HIGH);
      delay(2000);
      digitalWrite(E, LOW);
      int i = 0;
      feedback_d = digitalRead(feedback_D);
      lcd.clear();
      while (feedback_d != HIGH && i < 1000) {
        feedback_d = digitalRead(feedback_D);
        lcd.setCursor(0, 0);
        lcd.print("WAITING FOR");
        lcd.setCursor(0, 1);
        lcd.print("    FEEDBACK..");
        i++;
        delay(10);
      }
      if (feedback_d == HIGH) {
        break;
      }
      lcd.clear();
      j++;
    }
    if (j > 4) {
      while (true) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ERROR!");
        lcd.setCursor(0, 1);
        lcd.print("Please Reset");
        delay(1500);
      }
    }
    else {
      delay(10000);
      feedback_j = analogRead(feedback_J);
      lcd.clear();
      while (feedback_j > 100) {
        delay(50);
        feedback_j = analogRead(feedback_J);
        lcd.setCursor(0, 0);
        lcd.print("WAITING FOR");
        lcd.setCursor(0, 1);
        lcd.print("    FEEDBACK..");
        digitalWrite(I, HIGH);
      }
      digitalWrite(I, LOW);
    }
    state2 = 1;
    lcd.clear();
  }

  if (a_sw == 0 && b_sw == 0 && c_sw == 0 && state2 == 1 && on_off == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FRESH BOOT");
    lcd.setCursor(0, 1 );
    lcd.print("SQUENCE STARTED");
    delay(10000);
    feedback_k = analogRead(feedback_K);
    lcd.clear();
    int i = 0;
    state1 = 1;
    lcd.setCursor(0, 0);
    lcd.print("WAITING FOR");
    lcd.setCursor(0, 1);
    lcd.print("    FEEDBACK..");
    while ((feedback_k > 100 && state1 == 1) || i < 17001) {
      feedback_k = analogRead(feedback_K);
      delay(10);
      if (feedback_k < 100 || state1 == 0) {
        digitalWrite(H, LOW);
        state1 = 0;
        lcd.clear();
      }
      else {
        digitalWrite(H, HIGH);
      }
      if (i < 12000) {
        i++;
      }
      else if (i > 11998 && i < 17000) {
        digitalWrite(G, HIGH);
        i++;
      }
      else {
        digitalWrite(G, LOW);
        i++;
      }
    }
    digitalWrite(H, LOW);
    digitalWrite(G, LOW);
    state2 = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EVERYTHING IS OK");
  }
}
