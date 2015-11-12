#include <SPI.h>
#include "L6470.h"

L6470::L6470(int CSpin, int resetpin, int busypin)
: CSpin(CSpin), resetpin(resetpin), busypin(busypin)
{
	setup_SPI();
}

L6470::L6470(int CSpin, int resetpin)
: CSpin(CSpin), resetpin(resetpin), busypin(-1)
{
	setup_SPI();
}

void L6470::setup_SPI()
{
	pinMode(CSpin, OUTPUT);
	digitalWrite(CSpin, HIGH); //  until needed

	pinMode(resetpin, OUTPUT); // leave high/deasserted
	digitalWrite(resetpin, HIGH); //  deasserted

	if (busypin != -1)
		pinMode(busypin, INPUT_PULLUP);

	SPI.begin();

	SPISettings settings(5e6, MSBFIRST, SPI_MODE3);

}
