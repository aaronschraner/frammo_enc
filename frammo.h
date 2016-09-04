#ifndef FRAMMO_H
#define FRAMMO_H

#include "common.h"
#include "i2c.h"

//
//class for Adafruit I2C FRAM (Ferroelectric RAM) breakout board
class Frammo
{
	private:
		enum ReadWrite {
			_READ = 0x01,
			_WRITE = 0x00 };


		I2c i2c; //I2C interface
		byte devAddress; //slave device address (if multiple modules are sharing the I2C interface)
		void startCommand(ReadWrite rw);
		void sendAddress(uint16_t address);

	public:
		//constructor
		Frammo(Pin scl, Pin sda, byte devAddress = 0):
			i2c(scl, sda), devAddress(devAddress) {
				this->devAddress = 0;
			}

		void init(); //initialize pins
		void saveByte(uint16_t address, byte data); //save a byte at the given address
		byte readByte(); //read a byte at the current position
		byte readRand(uint16_t address); //read the byte at specified address
		void reset() { i2c.reset(); } //reset I2C interface
		uint32_t getProdID(); //get product ID info (doesn't work rn, not sure why)
		void blockWrite(uint16_t address, const void* dataIn, size_t len); //write a block of data to the FRAM 
		void blockRead(uint16_t address, void* dataOut, size_t len); //read a block of data from the FRAM
		void unInit() { i2c.unInit(); }
};

#endif
