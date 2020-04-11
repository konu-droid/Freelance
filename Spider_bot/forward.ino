void forward() {
  pos1();
  delay(500);
  pos2();
  delay(500);
  pos3();
  delay(500);
  pos4();
  delay(500);
  pos5();
  delay(500);
  pos6();
  delay(500);
  pos1();
}
void pos1()
{
  servo[0][0].write(90);   //LF
  servo[0][1].write(30);
  servo[0][2].write(30);
  servo[1][0].write(120);  //RF
  servo[1][1].write(140);
  servo[1][2].write(140);
  servo[2][0].write(90);  //LB
  servo[2][1].write(140);
  servo[2][2].write(140);
  servo[3][0].write(60);   //RB
  servo[3][1].write(30);
  servo[3][2].write(30);
}/*
  void pos1(){
  for(int i=1;i<=90;i++){
    servo[0][0].write(i);
    delay(15);
  }
  for(int i=1;i<=30;i++){
    servo[0][1].write(i);
    delay(15);
  }
  for(int i=1;i<=30;i++){
    servo[0][2].write(i);
    delay(15);
  }
  //-------------------
  for(int i=1;i<=120;i++){
    servo[1][0].write(i);
    delay(15);
  }
  for(int i=1;i<=140;i++){
    servo[1][1].write(i);
    delay(15);
  }
  for(int i=1;i<=140;i++){
    servo[1][2].write(i);
    delay(15);
  }
  //---------------------
  for(int i=1;i<=90;i++){
    servo[2][0].write(i);
    delay(15);
  }
  for(int i=1;i<=140;i++){
    servo[2][1].write(i);
    delay(15);
  }
  for(int i=1;i<=140;i++){
    servo[3][2].write(i);
    delay(15);
  }
  //-------------
  for(int i=1;i<=60;i++){
    servo[3][0].write(i);
    delay(15);
  }
  for(int i=1;i<=30;i++){
    servo[3][1].write(i);
    delay(15);
  }
  for(int i=1;i<=30;i++){
    servo[3][2].write(i);
    delay(15);
  }
  }*/
void pos2()
{ int k = 90, m = 30, n = 30;
  for (i = 0; i < 150; i++)
  { //lf leg
    if (i < 50) {
      servo[0][2].write(m);
      m++;
      delay(15);//+50=80
    }
    //lf base
    if (i > 50 && i < 80)
    {
      servo[0][0].write(k);
      k--;//-30=60
      delay(15);
    }
    //lf mid
    if (i > 120)
    {
      servo[0][1].write(n);
      n++;//+30=60
      delay(15);
    }
  }
}
/*
  void pos3() {
  servo[0][1].write(30);

  servo[0][2].write(30);

  servo[1][0].write(90);  //RF

  servo[2][0].write(130);  //LB

  servo[3][1].write(90);
  servo[3][2].write(100);
  }*/
void pos3() {
  int l = 60, m = 80, n = 120, o = 90, p = 30, q = 30;
  for (int i = 1; i <= 280; i++) {
    if (i >= 30) {
      servo[0][1].write(l);
      l--;//-30=30
      delay(15);
    }
    if (i > 30 && 80 >= i) {
      servo[0][2].write(m);
      m--;//-50=30
      delay(15);
    }
    if (i > 80 && 110 >= i) {
      servo[1][0].write(n);
      n--;//-30=90
      delay(15);
    }
    if (i > 110 && i <= 150) {
      servo[2][0].write(o);
      o++;//90+40=130
      delay(15);
    }
    if (i > 150 && i <= 210) {
      servo[3][1].write(p);
      p++;//30+60=90
      delay(15);
    }
    if (i > 210 && i <= 280) {
      servo[3][2].write(o);
      q++;//30+70=100
      delay(15);
    }
  }

}
/*
  void pos4()
  {
  servo[3][0].write(100);
  delay(500);
  servo[3][1].write(30);
  servo[3][2].write(30);

  }*/
void pos4()
{ int l = 60, m = 90, n = 100;
  for (int i = 1; i < 170; i++) {
    if (i <= 40) {
      servo[3][0].write(l);
      l++;//60+40=100
    }
    if (i > 40 && i < 100) {
      servo[3][1].write(m);
      m--;//90-60=30
    }
    if (i > 100) {
      servo[3][2].write(n);
      n--;//100-70=30
    }
  }
}

void pos5()
{
  k = 90; m = 140; n = 140;
  for (i = 0; i < 150; i++)
  { //rf leg
    if (i < 50) {
      servo[1][2].write(m);
      m--;//140-50=90
      delay(15);
    }
    //rf base
    if (i > 50 && i < 100)
    {
      servo[1][0].write(k);
      k++;//90+50=140
      delay(15);
    }
    //rf mid
    if (i > 120)
    {
      servo[1][1].write(n);
      n--;//140-30=110
      delay(15);
    }
  }
}/*
  void pos6()
  {
  servo[1][0].write(120);  //RF
  delay(500);
  servo[1][1].write(140);
  servo[1][2].write(140);
  servo[2][1].write(90);
  servo[2][2].write(80);

  servo[3][0].write(60);   //RB
  servo[3][1].write(30);
  servo[3][2].write(30);
  servo[0][0].write(90);   //LF
  servo[0][1].write(30);
  servo[0][2].write(30);
  }*/
void pos6() {

  int l = 90, m = 110, n = 90, o = 140, p = 140, q = 100, r = 100, s = 60;
  for (int i = 0; i < 200; i++) {
    if (i <= 30) {
      servo[1][0].write(l);
      l++;//90+30=120
    }
    if (i > 30 && i <= 60) {
      servo[1][1].write(m);
      m++;//110+30=140
    }
    if (i > 60 && i <= 110) {
      servo[1][2].write(n);
      n++;//90+50=140
    }
    if (i > 110 && i <= 160) {
      servo[1][1].write(o);
      o--;//110-30=90
    }
    if (i > 160 && i <= 220) {
      servo[2][1].write(p);
      p++;//140+60=200
    }
    if (i > 220 && i <= 280) {
      servo[2][2].write(q);
      q--;//140-60=80
    }
    if (i > 280 && i <= 350) {
      servo[3][2].write(r);
      r--;//100-70=30
    }
    if (i > 350 && i <= 380) {
      servo[0][0].write(s);
      s++;//60+30=90
    }
  }

}
/* void pos7(){
   }*/
