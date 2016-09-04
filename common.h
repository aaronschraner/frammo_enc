#ifndef COMMON_H
#define COMMON_H

#include <wiringPi.h>
#include <stdint.h>
#include <stdlib.h>

typedef int Pin;
typedef unsigned char byte;


void setPin(Pin pin, bool value);


#endif
