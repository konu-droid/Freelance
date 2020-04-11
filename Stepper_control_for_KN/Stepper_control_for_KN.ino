/* Include the library */
#include "HCMotor.h"
#include <Wire.h>
#include <EEPROM.h>

/* Pins used to drive the motors */
#define DIR_PIN A3 //Connect to drive modules 'direction' input.
#define CLK_PIN 9 //Connect to drive modules 'step' or 'CLK' input.

/* Set the analogue pin the potentiometer will be connected to. */
#define POT_PIN A0

/* Set a dead area at the centre of the pot where it crosses from forward to reverse */
#define DEADZONE 20

//mode switch to change from pot to switches
#define sw1 3
#define sw2 4
#define sw3 5


/* The analogue pin will return values between 0 and 1024 so divide this up between
   forward and reverse */
#define POT_REV_MIN 0
#define POT_REV_MAX (512 - DEADZONE)
#define POT_FWD_MIN (512 + DEADZONE)
#define POT_FWD_MAX 1024


/* Create an instance of the library */
HCMotor HCMotor;

int mychar = 30;
int count_up_limit1 = 200;   //change the number of steps to fit the no. of rotations
int count_down_limit1 = -200; //change the number of steps to fit the no. of rotations
int count_up_limit2 = 200;
int count_down_limit2 = -200;
int count1 = 0, count2 = 0; //do not change


void setup()
{
  Serial.begin(9600);
  /* Initialise the library */
  HCMotor.Init();

  /* Attach motor 0 to digital pins 8 & 9. The first parameter specifies the
     motor number, the second is the motor type, and the third and forth are the
     digital pins that will control the motor */
  HCMotor.attach(0, STEPPER, CLK_PIN, DIR_PIN);

  /* Set the number of steps to continuous so the the motor is always turning whilst
     not int he dead zone*/
  HCMotor.Steps(0, CONTINUOUS);

  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  pinMode(sw3, INPUT_PULLUP);

  byte two = EEPROM.read(201);
  byte one = EEPROM.read(202);

  int saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
  count1 = saving;
//
//  two = EEPROM.read(204);
//  one = EEPROM.read(205);
//  saving = ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
//  count2 = saving;
}


void loop()
{
  int Speed, Pot;
  Pot = analogRead(POT_PIN);

  if (digitalRead(sw3) == LOW) {
    if (digitalRead(sw1) == HIGH && digitalRead(sw2) == LOW && count1 > count_down_limit1) {
      HCMotor.Direction(0, REVERSE);
      Speed = Pot;
      count1--;
    }
    else if (digitalRead(sw1) == LOW && digitalRead(sw2) == HIGH && count1 < count_up_limit1)
    {
      HCMotor.Direction(0, FORWARD);
      Speed = Pot;
      count1++;
    }
    else {
      Speed = 0;
    }
  }
  else {
    if (digitalRead(sw1) == HIGH && digitalRead(sw2) == LOW && count2 > count_down_limit2) {
      HCMotor.Direction(0, REVERSE);
      Speed = Pot;
      count2--;
    }
    else if (digitalRead(sw1) == LOW && digitalRead(sw2) == HIGH && count2 < count_up_limit2)
    {
      HCMotor.Direction(0, FORWARD);
      Speed = Pot;
      count2++;
    }
    else {
      Speed = 0;
    }
  }

  /* Set the duty cycle of the clock signal in 100uS increments */
  HCMotor.DutyCycle(0, Speed);
  Serial.print("count1  ");
  Serial.print(count1);
  Serial.print("\t");
  Serial.print("count2  ");
  Serial.print(count2);
  Serial.print("\t");
  Serial.print("limits1  ");
  Serial.print(count_up_limit1);
  Serial.print("  ");
  Serial.print(count_down_limit1);
  Serial.print("\t");
  Serial.print("limits2  ");
  Serial.print(count_up_limit2);
  Serial.print("  ");
  Serial.println(count_down_limit2);

  //EEPROM update
  byte two = (count1 & 0xFF);
  byte one = ((count1 >> 8) & 0xFF);
  EEPROM.update(201, two);
  EEPROM.update(202, one);
//
//  two = (count2 & 0xFF);
//  one = ((count2 >> 8) & 0xFF);
//  EEPROM.update(204, two);
//  EEPROM.update(205, one);
}
