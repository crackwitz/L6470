#include <Arduino.h>
#include <SPI.h>
#include <L6470.h>

#include <avr/delay.h>

SPISettings spisettings(5000000, MSBFIRST, SPI_MODE3);

L6470::L6470(int CSpin, int resetpin, int busypin = -1)
: CSpin(CSpin), resetpin(resetpin), busypin(busypin)
{
	if (busypin != -1)
		pinMode(busypin, INPUT_PULLUP);

	pinMode(CSpin, OUTPUT);
	digitalWrite(CSpin, HIGH); //  until needed

	pinMode(resetpin, OUTPUT); // leave high/deasserted
	digitalWrite(resetpin, LOW);
	_delay_us(10); // t_STBY,min
	digitalWrite(resetpin, HIGH);
	_delay_us(650); // t_cpwu

	SPI.begin(); // preparing pins for SPI
}

uint8_t L6470::sendSPI(uint8_t data)
{
	SPI.beginTransaction(spisettings);

	digitalWrite(CSpin, LOW);
	uint8_t const received = SPI.transfer(data);
	digitalWrite(CSpin, HIGH);

	SPI.endTransaction();

	return received;
}

// uint32_t L6470::runCommand(cmd_def command)
// {

// }

uint32_t L6470::setParam(reg_def reg, uint32_t data)
{
	uint8_t const nbytes = (reg.bits-1) / 8 + 1;

	if (nbytes > 4)
		return -1;

	uint32_t mask = 0xffffffff >> (32-reg.bits);
	if (data > mask) data = mask;

	uint8_t *ptr = (uint8_t*)&data;

	sendSPI(reg.addr & 0x1f);
	
	for (int8_t i = nbytes-1; i >= 0; i -= 1)
		ptr[i] = sendSPI(ptr[i]);

	return data;
}

uint32_t L6470::getParam(reg_def reg)
{
	uint8_t const nbytes = (reg.bits-1) / 8 + 1;

	if (nbytes > 4)
		return -1;

	uint32_t data = 0;
	uint8_t *ptr = (uint8_t*)&data;

	sendSPI((reg.addr & 0x1f) | 0x20);

	for (int8_t i = nbytes-1; i >= 0; i -= 1)
		ptr[i] = sendSPI(0);

	return data;
}


void L6470::sendSPI_BE(uint8_t datalen, uint8_t *data)
{
	SPI.beginTransaction(spisettings);

	for (int8_t i = datalen-1; i >= 0; i -= 1)
	{
		digitalWrite(CSpin, LOW);
		data[i] = SPI.transfer(data[i]);
		digitalWrite(CSpin, HIGH);
		_delay_us(0.8); // T_disCS = 800ns
	}

	SPI.endTransaction();
}

uint32_t L6470::sendValue(uint8_t bits, uint32_t value)
{
	uint8_t const bytes = (bits-1) / 8 + 1;
	sendSPI_BE(bytes, (uint8_t*)&value);
	return value;
}

uint8_t L6470::cmdNop()
{
	return sendSPI(0x00);
}

void L6470::cmdRun(double steps)
{
	int8_t const dir = (steps >= 0);

	if (steps < 0)
		steps = -steps;

	command(CMD_RUN(dir), SPEED_VAL(steps) & 0xfffff);
}

void L6470::cmdRun(int val)
{
	cmdRun(val);
}

void L6470::cmdRun(int32_t val)
{
	int8_t const dir = (val >= 0);

	if (val < 0)
		val = -val;

	command(CMD_RUN(dir), val & 0xfffff);
}

uint32_t L6470::command(cmd_def command, uint32_t arg)
{
	sendSPI(command.code);
	arg &= (1ul << command.argbits) - 1;
	arg = sendValue(command.argbits, arg);
	return arg;
}
