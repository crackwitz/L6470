#pragma once

#include "Arduino.h"
#include "L6470_Defines.h"

class L6470
{
public:
	L6470(int CSpin, int resetpin, int busypin);
	L6470(int CSpin, int resetpin);

	uint8_t sendSPI(uint8_t data);

	uint32_t getParam(reg_def reg);
	uint32_t setParam(reg_def reg, uint32_t data);
	uint32_t sendValue(uint8_t bits, uint32_t value);
	uint32_t command(cmd_def command, uint32_t arg=0);

	uint8_t cmdNop();

private:
	int CSpin;
	int resetpin;
	int busypin;

	void setupSPI();

	void sendSPI_BE(uint8_t datalen, uint8_t *data);
};
