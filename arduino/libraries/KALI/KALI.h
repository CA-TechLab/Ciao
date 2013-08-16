#ifndef KALI_h
#define KALI_h

class KALIClass{
public:
	char len;//+:unsigned,-:signed
	char pnt;
	void *kreg,*ktmp;
	KALIClass();
	KALIClass& st(void *);
	KALIClass& ld(unsigned char *);
	KALIClass& ld(unsigned short *);
	KALIClass& ld(unsigned long *);
	KALIClass& ld(char *);
	KALIClass& ld(short *);
	KALIClass& ld(long *);
	KALIClass& sh(int);//plus means left shift
	KALIClass& neg();//change signum
	KALIClass& trm();//trim msb
	KALIClass& trl();//trim lsb
	KALIClass& exm();//expand msb
	KALIClass& exl();//expand lsb
	KALIClass& add(void *);
	KALIClass& sub(void *);
	KALIClass& x(unsigned char *);
	KALIClass& x(unsigned short *);
	KALIClass& x(char *);
	KALIClass& x(short *);
};

extern KALIClass Kali;

#endif
