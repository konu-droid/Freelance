#include<SoftwareSerial.h>

SoftwareSerial talk(13,12);
int percent,bat_lev;
long bat;
void setup() {
  Serial.begin(9600);
  talk.begin(9600);
pinMode(A7,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //bat_lev = analogRead(A7);
  for(int i=0;i<100;i++){
 bat_lev = analogRead(A7);
bat += bat_lev;
delay(100);
  }
  bat_lev=bat/100;
  bat=0;
if(bat_lev > 729){
  percent = map(bat_lev,729,806,60,100); 
  }
  else if(bat_lev > 720){
percent = map(bat_lev,720,729,20,60);    
    }
    else if(bat_lev > 691){
      percent = map(bat_lev,691,720,10,20);
      }
      else
      {
        percent = map(bat_lev,576,691,0,10);
        }
talk.println(percent);
Serial.println(bat_lev);
}
//576 - 3 - 0%
//691 - 3.6 - 10%
//720 - 3.75 - 20%
//729 - 3.8 - 60%
//806 - 4.2 - 100%
