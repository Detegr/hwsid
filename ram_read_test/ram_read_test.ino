#define ADATA 14
#define ALATCH 15
#define ACLOCK 16

#define CE1 10
#define CE2 11
#define OE 12
#define WE 13

#define D0 2
#define D1 3
#define D2 4
#define D3 5
#define D4 6
#define D5 7
#define D6 8
#define D7 9

#define READYPIN 19

void setup()
{
	for(int i=2; i<=19; ++i)
	{
		pinMode(i, INPUT);
	}
	Serial.begin(9200);
}

void setupPins()
{
	pinMode(ADATA, OUTPUT);
	pinMode(ALATCH, OUTPUT);
	pinMode(ACLOCK, OUTPUT);

	for(int i=D0; i<=D7; ++i)
	{
		pinMode(i, INPUT);
	}

	pinMode(CE1, OUTPUT);
	pinMode(CE2, OUTPUT);
	pinMode(OE, OUTPUT);
	pinMode(WE, OUTPUT);

	pinMode(READYPIN, INPUT);

	setHighImpedanceMode();
}

void setHighImpedanceMode()
{
	digitalWrite(CE1, HIGH);
	digitalWrite(CE2, LOW);
	digitalWrite(OE, HIGH);
	digitalWrite(WE, HIGH);
}

void setReadMode()
{
	digitalWrite(CE1, LOW);
	digitalWrite(CE2, HIGH);
	digitalWrite(WE, HIGH);
	delayMicroseconds(1);
	digitalWrite(OE, LOW);
}

void shift16(int data, int clock, uint16_t val)
{
	shiftOut(data, clock, LSBFIRST, (val & 0xFF00) >> 8);
	shiftOut(data, clock, LSBFIRST, val & 0x00FF);
}

uint8_t readRAM(uint16_t address)
{
	setHighImpedanceMode();

	digitalWrite(ALATCH, LOW);
	shift16(ADATA, ACLOCK, address);
	digitalWrite(ALATCH, HIGH);

	setReadMode();
	delayMicroseconds(10);

	return readInputValue();
}

uint8_t readInputValue()
{
	uint8_t ret=0;
	for(int i=D7; i>=D0; --i)
	{
		int val=digitalRead(i);
		ret <<= 1;
		ret |= (val & 0xFF);
	}
	return ret;
}

#define LOCS 20000

uint8_t iter;
uint8_t arr[]={1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,0};

void loop()
{
	Serial.println("Waiting for high");
	while(digitalRead(READYPIN) == LOW) {}
	Serial.println("Waiting for low");
	while(digitalRead(READYPIN) == HIGH) { delay(100); }
	Serial.println("Starting to read");
	delay(1000);
	setupPins();
	for(uint16_t i=0; i<LOCS; ++i)
	{
		Serial.print("Read [");
		Serial.print(i);
		Serial.print("]: ");
		Serial.println(readRAM(i));
	}
}
