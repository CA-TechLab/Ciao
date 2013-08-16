#include <JPC.h>
#include <Firmata.h>
#include <FJAX.h>

struct{
  short A,B;
} stat;
struct{
  short A,B;
} mode;

word tp[]={28000,25000,28000,25000,28000,14000,28000};

void *seg[]={&stat,&mode,&tp,&JPCA,&JPCB};

void setup(){
//   pinMode(13,OUTPUT);
  Firmata.begin(57600);
  FJAX.setup(&Firmata,seg);
  JPCA.vref=300;
  JPCB.vref=200;
  mode.A=mode.B=0;
}
void loop(){
  stat.A=JPCA.check();
  stat.B=JPCB.check();
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
  
  if(stat.A==0){
    if(mode.A<0){
      if(mode.A==-1) analogWrite(10,0);
      else analogWrite(10,random(255));
      JPCA.timer(100);
      mode.A++;
    }
    else if(mode.A>=7*2){ JPCA.timer(1000L*3600*12); mode.A=-20;}
    else if(mode.A&1){ JPCA.go(tp[mode.A/2]); mode.A++;}
    else{ JPCA.timer(100); mode.A++;}
  }

  if(stat.B==0){
    switch(mode.B){
    case 0:
      JPCB.go(10000);
      mode.B++;
      break;
    case 1:
      JPCB.timer(100);
      mode.B++;
      break;
    case 2:
      JPCB.go(20000);
      mode.B++;
      break;
    case 3:
      JPCB.timer(5000);
      mode.B=0;
      break;
    }
  }
}

