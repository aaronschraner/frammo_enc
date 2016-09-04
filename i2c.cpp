#include "i2c.h"

void I2c::init()
{
	pinMode(scl, OUTPUT);
	pinMode(sda, OUTPUT);
	setPin(scl, 1);
	setPin(sda, 1);
}

//assume sda=1, scl=1
void I2c::start()
{
	setPin(sda, 1);
	setPin(scl, 1);
	setPin(sda, 0); //start
}

//assume sda=0, scl=1
void I2c::stop()
{
	setPin(sda, 1);
}

//assume scl=1, sda=0
bool I2c::sendByte(byte val)
{
	for(byte x = 0x80; x; x>>=1)
	{
		setPin(scl, 0);
		setPin(sda, val & x);
		setPin(scl, 1);
	}
	setPin(scl, 0);
	pinMode(sda, INPUT);
	setPin(scl, 1);
	bool ack = digitalRead(sda);
	setPin(scl, 0);
	pinMode(sda, OUTPUT);
	return ack;
}
//leave with sda=?, scl=0

//start with SCL=0, sda=?
byte I2c::readByte(bool ack)
{
	pinMode(sda, INPUT);
	byte result = 0;
	for(byte x = 0x80; x; x>>=1)
	{
		setPin(scl, 1);
		result |= digitalRead(sda) ? x : 0;
		setPin(scl, 0);
	}
	pinMode(sda, OUTPUT);
	setPin(sda, ack);
	setPin(scl, 1);
	setPin(scl, 0);
	return result;
}
//end with scl=0, sda=ACK

void I2c::reset()
{
	pinMode(sda, INPUT);
	setPin(sda, 0);
	for(int i=0; i < 9; i++)
	{
		pinMode(sda, OUTPUT);
		setPin(scl, 0);
		pinMode(sda, INPUT);
		setPin(scl, 1);
	}
	setPin(sda, 1);
	setPin(scl, 1);
	pinMode(sda, OUTPUT);

}

void I2c::unInit()
{
	pinMode(sda, INPUT);
	pinMode(scl, INPUT);
	setPin(sda, 0);
	setPin(scl, 0);
}
