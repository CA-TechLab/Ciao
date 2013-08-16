#include <JPC.h>
#include <Firmata.h>
#include <FJAX.h>

void setup(){
//   pinMode(13,OUTPUT);
  Firmata.begin(57600);
  FJAX.setup(&Firmata,&JPCA);
  JPCA.vref=300;
}

word tp[]={28000,14000,28000,5000,27000,5000,27000};

void loop(){
  static int stepA=-50;
  static int stepB=0;
  int statA=JPCA.check();
  int statB=JPCB.check();
  FJAX.check();
/*  if(JPCA.stat&0x10){
    static int w=0;
    if(w>50){
      Serial.print(JPCA.xfb);
      Serial.println("\t");
      w=0;
    }
    else w++;
  }*/
  
  if(statA==0){
    if(stepA<0){
      if(stepA==-1) analogWrite(13,0);
      else analogWrite(13,random(255));
      JPCA.timer(100);
      stepA++;
    }
    else if(stepA>=3*2){ JPCA.timer(1000L*3600*12); stepA=-50;}
    else if(stepA&1){ JPCA.go(tp[stepA/2]); stepA++;}
    else{ JPCA.timer(100); stepA++;}
  }
  else if(statA<0){ analogWrite(13,255);}

  if(statB==0){
    if(stepB==0){
      JPCB.pwm(0);
      JPCB.timer(90000);
      stepB++;
    }
    else{
      JPCB.pwm(250);
      JPCB.timer(30000);
      stepB=0;
    }
  }
}

