#ifndef JPC_h
#define JPC_h

class JPCClass{
public:
	short xfb,vfb,xref,vref;
	unsigned short dt,kvfb,krev,duty;
	long wdt,tlaps,tsamp,spos,scount;
	unsigned char stat,pdir,pmot,ppos;

	JPCClass(int,int,int);
	void stop(void);
	void go(unsigned int);
	void timer(long);
	void pwm(int);
	int check(void);	//0:stopped,1:working,-1:error/error cleared by "go"
};

extern JPCClass JPCA,JPCB;

#endif
