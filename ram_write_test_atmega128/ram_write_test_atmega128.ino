#include <avr/sleep.h>

#define CE1 14
#define CE2 11
#define OE 13
#define WE 12

#define ADATA 10
#define ALATCH 15
#define ACLOCK 16

#define DDATA 19
#define DLATCH 18
#define DCLOCK 16

#define DATAOUT 17

void setup()
{
	pinMode(0, OUTPUT);
	pinMode(ADATA, OUTPUT);
	pinMode(ALATCH, OUTPUT);
	pinMode(ACLOCK, OUTPUT);
	pinMode(DDATA, OUTPUT);
	pinMode(DLATCH, OUTPUT);
	pinMode(DCLOCK, OUTPUT);
	pinMode(DATAOUT, OUTPUT);

	pinMode(CE1, OUTPUT);
	pinMode(CE2, OUTPUT);
	pinMode(OE, OUTPUT);
	pinMode(WE, OUTPUT);

	digitalWrite(DATAOUT, LOW);

	setHighImpedanceMode();
}

inline void prepareForSleep()
{
	for(int i=0; i<=19; ++i)
	{
		pinMode(i, INPUT);
	}
}

inline void setHighImpedanceMode()
{
	digitalWrite(CE1, HIGH);
	digitalWrite(CE2, LOW);
	digitalWrite(OE, HIGH);
	digitalWrite(WE, HIGH);
}

inline void setWriteMode()
{
	digitalWrite(CE2, HIGH);
	digitalWrite(WE, LOW);
	delayMicroseconds(1);
	digitalWrite(CE1, LOW);
}

inline void disableDataOut()
{
	digitalWrite(DATAOUT, LOW);
}

inline void enableDataOut()
{
	digitalWrite(DATAOUT, HIGH);
}

inline void shift16(int data, int clock, uint16_t val)
{
	shiftOut(data, clock, LSBFIRST, (val & 0xFF00) >> 8);
	shiftOut(data, clock, LSBFIRST, val & 0x00FF);
}

inline void writeRAM(uint16_t address, uint8_t val)
{
	setHighImpedanceMode();

	digitalWrite(ALATCH, LOW);
	shift16(ADATA, ACLOCK, address);
	digitalWrite(ALATCH, HIGH);

	enableDataOut();
	digitalWrite(DLATCH, LOW);
	shiftOut(DDATA, DCLOCK, MSBFIRST, val);
	digitalWrite(DLATCH, HIGH);

	setWriteMode();
	delayMicroseconds(10);
}

inline void gotoSleep()
{
	setHighImpedanceMode();
	disableDataOut();
	digitalWrite(0, LOW);
	prepareForSleep();
	cli();
	sleep_enable();
	sleep_cpu();
}

#define LOCS 20000

uint8_t iter;
uint8_t arr[]={1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,0};

void loop()
{
	digitalWrite(0, HIGH);
	writeRAM(0, 111);
	for(uint16_t i=1; i<LOCS; i++, ++iter)
	{
		writeRAM(i, arr[iter%20]);
	}
	gotoSleep();
}
