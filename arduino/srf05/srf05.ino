#include <Firmata.h>
#include <FJAX.h>

struct {
  long tlac;
  word step;
} pm;
long mes[4];

void *seg[]={&pm,mes};

void setup(){
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,INPUT);
  digitalWrite(10,HIGH);
  Firmata.begin(57600);
  FJAX.setup(&Firmata,seg);
  pm.step=0;
}
void loop(){
  FJAX.check();
  
  long tu=micros();
  
  switch(pm.step){
  case 0:
    mes[0]=mes[1]=mes[2]=0;
    pm.tlac=tu;
    digitalWrite(11,HIGH);
    pm.step++;
    break;
  case 1:
    if((mes[0]=tu-pm.tlac)<10) break;
    digitalWrite(11,LOW);
    pm.tlac=tu;
    pm.step++;
    break;
  case 2:
    mes[1]=tu-pm.tlac;
    if(!digitalRead(12)) break;
    pm.tlac=tu;
    pm.step++;
    break;
  case 3:
    mes[2]=tu-pm.tlac;
    if(digitalRead(12)) break;
    pm.tlac=tu;
    pm.step++;
    break;
  case 4:
    mes[3]=tu-pm.tlac;
    if(!digitalRead(12)) break;
    pm.tlac=tu;
    pm.step++;
    break;
  }
}

