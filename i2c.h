#ifndef I2C_H
#define I2C_H

#include "common.h"

class I2c
{
	private:
		Pin scl, sda;

	public:
		I2c(Pin scl, Pin sda):
			scl(scl), sda(sda) {}
		void init(); //initialize pins
		void start(); //send start signal
		void stop(); //send stop signal
		bool sendByte(byte val);
		byte readByte(bool ack);
		void reset();
		void unInit();
		
};

#endif

