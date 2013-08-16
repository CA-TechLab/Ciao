#include <Arduino.h>
#include <../../../../libraries/Firmata/Firmata.h>
#include "FJAX.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

/******************************************************************************
 * Constructors
 ******************************************************************************/
static FirmataClass *fata;
FJAXClass::FJAXClass(){}
/******************************************************************************
 * User API
 ******************************************************************************/
static void sysexCallback(byte command, byte argc, byte *argv);
void FJAXClass::setup(FirmataClass *f,void **adds){
  fata=f;
  baseAdds=adds;
  fata->setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);
  fata->attach(START_SYSEX, sysexCallback);
}
void FJAXClass::check(){
  while(fata->available())
    fata->processInput();
}

FJAXClass FJAX;

static const byte hexmap[]={"0123456789ABCDEF"};
static unsigned long decode(byte *argv){
  unsigned long ret=0;
  for(;;argv++){
    byte b=*argv;
    if('0'<=b && b<='9'){
      ret<<=4;
      ret|=b-'0';
    }
    else if('A'<=b && b<='F'){
      ret<<=4;
      ret|=b+10-'A';
    }
    else if('a'<=b && b<='f'){
      ret<<=4;
      ret|=b+10-'a';
    }
    else return ret;
  }
}
static void sendData(unsigned long l){
  boolean send=false;
  for(int i=0;i<4;i++,l<<=8){
    byte b=(l&0xff000000)>>24;
    byte nm=(b&0xf0)>>4;
    if(nm) send=true;
    if(send) Serial.write(hexmap[nm]);
    byte nl=b&0x0f;
    if(nl) send=true;
    if(send) Serial.write(hexmap[nl]);
  }
  if(!send) Serial.write('0');
}

static void sysexCallback(byte command, byte argc, byte *argv){
  word seg=argv[0];
  word offset=argv[1];
  unsigned long data=0;
  argv[argc]=0;
  switch(command){
  case 0x50:
    Serial.write(0xf0);
    Serial.write(0x57);
    sendData(*((byte *)(FJAX.baseAdds[seg])+offset));
    Serial.write(0xf7);
    break;
  case 0x51:
    Serial.write(0xf0);
    Serial.write(0x57);
    sendData(*(word *)(((byte *)(FJAX.baseAdds[seg])+offset)));
    Serial.write(0xf7);
    break;
  case 0x52:
    Serial.write(0xf0);
    Serial.write(0x57);
    sendData(*(unsigned long *)(((byte *)(FJAX.baseAdds[seg])+offset)));
    Serial.write(0xf7);
    break;
  case 0x54:
    Serial.write(0xf0);
    Serial.write(0x57);
    data=decode(argv+2);
    *((byte *)(FJAX.baseAdds[seg])+offset)=data;
    sendData(data);
    Serial.write(0xf7);
    break;
  case 0x55:
    Serial.write(0xf0);
    Serial.write(0x57);
    data=decode(argv+2);
    *(word *)((byte *)(FJAX.baseAdds[seg])+offset)=data;
    sendData(data);
    Serial.write(0xf7);
    break;
  case 0x56:
    Serial.write(0xf0);
    Serial.write(0x57);
    data=decode(argv+2);
    *(unsigned long *)((byte *)(FJAX.baseAdds[seg])+offset)=data;
    sendData(data);
    Serial.write(0xf7);
    break;
  }
}
