#ifndef FJAX_h
#define FJAX_h

class FirmataClass;

class FJAXClass{
public:
	void **baseAdds;
	FJAXClass();
	void setup(FirmataClass *,void **);
	void check();
};

extern FJAXClass FJAX;

#endif
