#include "common.h"
#include <iostream>

void setPin(Pin pin, bool value)
{
	//int oldv = digitalRead(pin);
	delayMicroseconds(10);
	digitalWrite(pin, value);
	delayMicroseconds(10);
//	if(pin == 1)
//		std::cout << "_/\\^" [oldv * 2 + (int)value] ;
//	else
//		std::cout << "_^"[(int)digitalRead(1)];
}

