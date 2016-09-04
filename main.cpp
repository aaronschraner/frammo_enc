#include <iostream>
#include <iomanip>
#include "common.h"
#include "frammo.h"
#include <cstring>
#include <cctype>
#include "sdes.h"
#include <sstream>

using namespace std;

int main()
{
	if(wiringPiSetup() == -1)
		return -1;

	Frammo frammo(2, 0);
	frammo.init();

	cout << "Enter a hexadecimal string to store into FRAM.  (no spaces) \n> ";
	string resp;
	getline(cin, resp);
	int msglength;
	char* message = new char[msglength = resp.length() / 2];
	for(int i=0; i<resp.length() / 2; i++)
	{
		stringstream ss(resp.substr(i*2, 2));
		int v;
		ss >> hex >> v;
		message[i] = v;
	}

	cout << "Do you want to encrypt this message? [y/n] > ";
	char yn;
	cin >> yn;
	if(tolower(yn) == 'y')
	{

		cout << "What SDES 10 bit key would you like to use? (enter as a 10 digit binary number)\n> ";
		uint16_t key = 0;
		for(uint16_t x = 0x200; x; x>>=1)
		{
			cin >> yn;
			key |= (yn == '1') ? x : 0;
		}

		cout << "Encrypting with key 0x" << hex << key << ". " << endl;

		for(int i=0; i<msglength; i++)
			message[i] = sdes::encrypt(message[i], key);

		cout << "Ciphertext: [ ";
		for(int i=0; i < msglength; i++)
			cout << hex << (int)message[i] << " ";
		cout << "]" << endl;


	}
	else
	{
		cout << "Not encrypting. " << endl;
	}
	frammo.reset();
	delay(1);

	uint16_t address = 0x40;
	cout << "Writing data to FRAM address 0x" << hex << address << "..." << flush;
	frammo.blockWrite(address, message, msglength ); 
	cout << endl;
	//frammo.blockWrite(0x80, message, strlen(message) + 1);

	cout << "FRAM content dump (0x40 - 0x4F): " << endl;
	char* buffer = new char[16]; //16 bytes of buffer space
	frammo.blockRead(address, buffer, 16);
	for(int i = 0; i < 16; i++)
		cout << setw(2) << setfill('0') << hex << (int)buffer[i] << " ";
	cout << "[";
	for(int i = 0; i < 16; i++)
		cout << " " << (isprint(buffer[i]) ? buffer[i] : '?') ;
	cout << " ]" << endl;

	//for(int block = 0; block < 0x10; block++)
	//{
	//	frammo.blockRead(block << 4, buffer, 16);
	//	cout << "[ " << setw(4) << setfill('0') << hex << block * 16 << " ]: ";
	//	for(int i = 0; i < 16; i++)
	//		cout << setw(2) << setfill('0') << hex << (int)buffer[i] << " ";
	//	cout << "[";
	//	for(int i = 0; i < 16; i++)
	//		cout << " " << (isprint(buffer[i]) ? buffer[i] : '?') ;
	//	cout << " ]";
	//	cout << endl;
	//}
	delete[] buffer;
	delete[] message;

	frammo.unInit();




	//cout << hex << (int)frammo.readRand(0x10) << endl;

	return 0;
}

