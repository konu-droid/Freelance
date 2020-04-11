void sit(){
  for (i=20,j=160;i<140;i++,j--){
    servo[1][1].write(i); //RF
    servo[1][2].write(i);
    servo[2][1].write(i-5); //LB
    servo[2][2].write(i-5);
    servo[0][1].write(j);//LF
    servo[0][2].write(j);
    servo[3][1].write(j);//RB
    servo[3][2].write(j);

    delay(15);
    }
  }

  void stand()
{
  // for one set of diagonal legs
  for (i=140,j=40;i>20;i--,j++){
    servo[1][1].write(i); //RF
    servo[1][2].write(i);
    servo[2][1].write(i+5); //LB
    servo[2][2].write(i+5);
    servo[0][1].write(j);//LF
    servo[0][2].write(j);
    servo[3][1].write(j);//RB
    servo[3][2].write(j);
    delay(15);
    }
  }
