#include "Arduino.h"
#include "JPC.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

/******************************************************************************
 * Constructors
 ******************************************************************************/
JPCClass::JPCClass(int d,int m,int p){
	wdt=tlaps=stat=xfb=vfb=0;
	pdir=d;
	pmot=m;
	ppos=p;
	kvfb=20;
	krev=15;
	dt=10;
	vref=300;
	tsamp=-10000;
}

/******************************************************************************
 * User API
 ******************************************************************************/

void JPCClass::go(unsigned int dest){
	long t=millis();
	if(t-tsamp>1000){
		word x=analogRead(ppos);
		xfb=x<<5;
		vfb=0;
		spos=x;
		scount=1;
	}
	tlaps=t;
	tsamp=tlaps+dt;
	wdt=constrain(2L*abs(xfb-dest)*dt/vref,1000,10000);
	xref=dest;
	pwm(0);
	pinMode(pdir,OUTPUT);
	if(xref>xfb){
		stat=2;
		digitalWrite(pdir,LOW);
	}
	else if(xref<xfb){
		stat=4;
		digitalWrite(pdir,HIGH);
	}
}
void JPCClass::stop(void){
	stat&=0xF1;
	pwm(0);
}
void JPCClass::timer(long t){
	if(!(stat&6)){
	 	wdt=t;
		tlaps=millis();
		stat=8;
	}
}
void JPCClass::pwm(int d){ analogWrite(pmot,duty=constrain(d,0,255));}
int JPCClass::check(void){
	long t=millis();
	spos+=analogRead(ppos);
	scount++;
	if(t>tsamp){
		long x=(spos<<5)/scount;
		vfb=x-xfb;
		xfb=x;
		spos=scount=0;
		tsamp=t+dt;
		stat|=0x10;
	}
	else stat&=0xEF;
	if(stat&1) return -1;
	if(stat&8){
		if(t-tlaps>wdt){ stat=0;return 0;}
		else return 1;
	}
	if(stat&6){//moving
		if(t-tlaps>wdt){ stop();stat|=1;return -1;}
		else goto cont;
	}
	else return 0;
cont:
	if(stat&0x10){
		int d=0;
		if(stat&2){
			d=(-long(kvfb)*vfb+long(krev+kvfb)*vref)>>5;
			if(xfb>=xref){ stop(); return 0;}
		}
		else if(stat&4){
			d=(long(kvfb)*vfb+long(krev+kvfb)*vref)>>5;
			if(xfb<=xref){ stop(); return 0;}
		}
		pwm(d);
	}
	return 1;
}

JPCClass JPCA(12,3,2);
JPCClass JPCB(13,11,3);
