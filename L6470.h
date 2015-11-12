#pragma once

#include "L6470_Registers.h"
#include "Arduino.h"

class L6470
{
public:
	L6470(int CSpin, int resetpin, int busypin);
	L6470(int CSpin, int resetpin);

private:
	int CSpin;
	int resetpin;
	int busypin;

	void setup_SPI();
};
