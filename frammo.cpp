#include "frammo.h"

//initialize FRAM pins
void Frammo::init()
{
	i2c.init();
}

//start a read/write operation
// (send I2C start sequence followed by command byte)
void Frammo::startCommand(ReadWrite rw)
{
	i2c.start();
	i2c.sendByte(0b10100000 | devAddress << 1 | rw);
}

//send a 16-bit address via I2C.
void Frammo::sendAddress(uint16_t address)
{
	i2c.sendByte(address >> 8);
	i2c.sendByte(address & 0xFF);
}

//save a byte of data at the specified address in the FRAM
void Frammo::saveByte(uint16_t address, byte data)
{
	//send the write command string (b1010AAA0, A = device address)
	startCommand(_WRITE);

	//send the address to write to (in the FRAM)
	sendAddress(address);

	//send the one byte of data
	i2c.sendByte(data);

	//end this I2C operation
	i2c.stop();
}

//read the byte at the current location in memory
byte Frammo::readByte()
{
	startCommand(_READ);
	byte result = i2c.readByte(1);
	i2c.stop();
	return result;
}

//read data from a random location in the FRAM (non-sequential read)
byte Frammo::readRand(uint16_t address)
{
	//send address
	startCommand(_WRITE);
	sendAddress(address);

	//read a byte of data
	startCommand(_READ);
	byte result = i2c.readByte(1);

	//stop I2C operation
	i2c.stop();
	return result;
}

//TODO: this is broken
//return the product ID information
uint32_t Frammo::getProdID()
{
	i2c.start();
	i2c.sendByte(0xF8);
	i2c.sendByte(0xA0);
	i2c.start();
	i2c.sendByte(0xF9);
	int d[3];
	d[0] = i2c.readByte(0);
	d[1] = i2c.readByte(0);
	d[2] = i2c.readByte(1);
	i2c.stop();
	return d[0] << 16 | d[1] << 8 | d[2];
}

//write a block of data to the FRAM module
//  takes:
//    address - the address in FRAM to start writing to
//    data    - pointer to the memory that should be copied into the FRAM
//    len     - the size of the page to be written. (data[0..len-1] is written)
void Frammo::blockWrite(uint16_t address, const void* data, size_t len)
{
	//get setup for block operation
	startCommand(_WRITE);
	
	//send the address to be written to
	sendAddress(address);

	//send the data one byte at a time
	for(size_t i = 0; i < len; i++)
		i2c.sendByte(*((byte*)data + i));

	//stop I2c operation
	i2c.stop();
}

//read a block of data from the FRAM module into the host program
//  takes:
//    address - the address in FRAM to begin reading from
//    dataOut - pointer to where the data should be copied to in system memory
//    len     - the length of the section of data that should be copied
//
void Frammo::blockRead(uint16_t address, void* dataOut, size_t len)
{
	//get setup for block operation
	startCommand(_WRITE);

	//send the address being read from
	sendAddress(address);

	//start a read operation
	startCommand(_READ);

	//read the data into dataOut one byte at a time
	for(size_t i = 0; i < len; i++)
	{
		((byte*)dataOut)[i] = i2c.readByte(i == len - 1);
	}

	//stop the I2c operation
	i2c.stop();
}


