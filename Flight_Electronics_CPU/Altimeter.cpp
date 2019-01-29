/*
 * Altimeter.cpp
 *
 *  Created on: Jan 28, 2019
 *      Author: erics
 */

#include "Altimeter.h"

void Altimeter::init()
{
	Wire.setSCL(33);
	Wire.setSDA(34);
	Wire.begin(); // this might be the second time we call wire.begin... we need to make sure it doesn't cause problems

	velocity = 0;
	pressure = 0;

	send16BitWord(0xEFC8); // ask for part number
	uint8_t buf[2] = {0};
	readBytes(buf, 2);
	if(buf[0]|buf[1])
	{
		uint8_t partNumber = ((buf[0] << 8) | buf[1]) & 0b11111;

		char buffer[5];
		itoa(partNumber & 0b11111, buffer, 2);
		Telemetry::printf(MSG_INFO, "Altimeter detected: ICP-%s\n", buffer);
		send16BitWord(0x805D); // soft reset
	}
	else
	{
		Telemetry::printf(MSG_ERROR, "Altimeter NOT detected!\n");
	}

}

void Altimeter::send16BitWord(uint16_t word)
{
	Wire.beginTransmission(ALTIMETER_ADDRESS);
	Wire.send((word >> 8) & 0xFF);
	Wire.send(word & 0xFF);
	Wire.endTransmission();
}

void Altimeter::readBytes(uint8_t* buf, uint8_t len)
{
	Wire.requestFrom(ALTIMETER_ADDRESS, 2);
	if(Wire.available() >= len+1)
	{
		for(int i=0; i<len; i++)
		{
			buf[i] = Wire.read();
		}
		uint8_t crc = Wire.read();
		if(generateCRC8(buf, len) != crc) Telemetry::printf(MSG_ERROR, "Altimeter CRC ERROR!\n");
	}
}

uint8_t Altimeter::generateCRC8(const uint8_t *bytes, uint8_t length)
{
	return CRC8.generic(0x31, 0xFF, CRC_FLAG_NOREFLECT, bytes, length);
}
