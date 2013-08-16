#include <Firmata.h>

#define MAX_QUERIES 8
#define MINIMUM_SAMPLING_INTERVAL 10

#define REGISTER_NOT_SPECIFIED -1:

/*JGD(Jurduino Global Data)*/
long adds;
long tpre=0;
int dbug;
byte buf[]={"0123456789"};

const byte hexmap[]={"0123456789ABCDEF"};
void *jBaseAdds=&adds;
unsigned long decode(byte *argv){
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
    else return ret;
  }
}
void sendData(unsigned long l){
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
void sendString(byte *s,int n){
  for(int i=0;i<n;i++,s++){
    Serial.write(*s);
  }
}

void sysexCallback(byte command, byte argc, byte *argv){
  word offset=argv[0];
  unsigned long data=0;
  argv[argc]=0;
  switch(command){
  case 0x50:
    Serial.write(0xf0);
    Serial.write(0x50);
    sendData(*((byte *)(jBaseAdds)+offset));
    Serial.write(0xf7);
    break;
  case 0x51:
    Serial.write(0xf0);
    Serial.write(0x50);
    sendData(*(word *)(((byte *)(jBaseAdds)+offset)));
    Serial.write(0xf7);
    break;
  case 0x52:
    Serial.write(0xf0);
    Serial.write(0x50);
    sendData(*(unsigned long *)(((byte *)(jBaseAdds)+offset)));
    Serial.write(0xf7);
    break;
  case 0x54:
    Serial.write(0xf0);
    Serial.write(0x50);
    data=decode(argv+1);
    *((byte *)(jBaseAdds)+offset)=data;
    sendData(data);
    Serial.write(0xf7);
    break;
  case 0x55:
    Serial.write(0xf0);
    Serial.write(0x50);
    data=decode(argv+1);
    *(word *)((byte *)(jBaseAdds)+offset)=data;
    sendData(data);
    Serial.write(0xf7);
    break;
  case 0x56:
    Serial.write(0xf0);
    Serial.write(0x50);
    data=decode(argv+1);
    *(unsigned long *)((byte *)(jBaseAdds)+offset)=data;
    sendData(data);
    Serial.write(0xf7);
    break;
  }
}

void setup(){
  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);

  Firmata.attach(START_SYSEX, sysexCallback);

  Firmata.begin(57600);
  
  adds=0x1234;
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop(){
  long t=micros();
  dbug=t-tpre;
  tpre=t;

  while(Firmata.available())
    Firmata.processInput();
}

