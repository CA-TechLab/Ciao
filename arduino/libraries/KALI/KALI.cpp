#include "Arduino.h"
#include "KALI.h"
#include <string.h>

static unsigned char buf[12];

KALIClass::KALIClass(){
	kreg=buf;
	ktmp=buf+8;
	len=1;
}
KALIClass& KALIClass::trm(){
	return *this;
}
KALIClass& KALIClass::trl(){
	return *this;
}
KALIClass& KALIClass::exm(){
	return *this;
}
KALIClass& KALIClass::exl(){
	return *this;
}
KALIClass& KALIClass::ld(unsigned char *a){
	memcpy(kreg,a,len=1);
	return *this;
}
KALIClass& KALIClass::ld(unsigned short *){
	return *this;
}
KALIClass& KALIClass::ld(unsigned long *){
	return *this;
}
KALIClass& KALIClass::ld(char *){
	return *this;
}
KALIClass& KALIClass::ld(short *){
	return *this;
}
KALIClass& KALIClass::ld(long *){
	return *this;
}
KALIClass& KALIClass::st(void *){
	return *this;
}
KALIClass& KALIClass::sh(int){
	return *this;
}
KALIClass& KALIClass::add(void *){
	return *this;
}
KALIClass& KALIClass::sub(void *){
	return *this;
}
KALIClass& KALIClass::x(unsigned char *b){
	*kreg*=*b;
	return *this;
}
KALIClass& KALIClass::x(unsigned short *){
	return *this;
}
KALIClass& KALIClass::x(char *){
	return *this;
}
KALIClass& KALIClass::x(short *){
	return *this;
}

KALIClass Kali;
