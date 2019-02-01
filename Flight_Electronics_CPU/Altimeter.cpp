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
	Wire.setClock(I2C_CLOCK_SPEED);
	Wire.setTimeout(I2C_TIMEOUT);

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
	}
	else
	{
		Telemetry::printf(MSG_ERROR, "Altimeter NOT detected!\n");
		return;
	}


	send16BitWord(0x805D); // soft reset
	if(!updateOPT()) Telemetry::printf(MSG_ERROR, "Unable to get Altimeter OPT params!\n");
}

bool Altimeter::updateOPT()
{
	// get OPT calibration parameters

	send16BitWord(0xC595); // move register pointer
	send16BitWordWithCRC(0x0066, 0x9C); // set up OPT Read
	for(int i=0; i<4; i++)
	{
		send16BitWord(0xC7F7); // increment OPT register pointer
		uint8_t cnbuf[2] = {0};
		if(readBytes(cnbuf, 2))
			cn[i] = cnbuf[0] << 8 | cnbuf[1];
		else
			return false;
	}
	return true;
}

void Altimeter::getNewSample()
{
	if(send16BitWord(ALTIMETER_MODE_NORMAL)) return; // checking for ACK at end of transmission
	for(int i=0; i<10; i++) {
		//try to read from altimeter
		delayMicroseconds(100);
		uint8_t buf[6] = {0};
		if(Wire.readBytes(buf, 2))
		{
			if(Wire.readBytes(buf+2, 2))
			{
				if(Wire.readBytes(buf+4, 2))
				{
					uint8_t P_MMSB = buf[0];
					uint8_t P_MLSB = buf[1];
					uint8_t P_LMSB = buf[2];
					uint8_t P_LLSB = buf[3];
					uint8_t T_MSB = buf[4];
					uint8_t T_LSB = buf[5];
					p_dout = P_MMSB << 16 | P_MLSB << 8 | P_LMSB;
					t_dout = T_MSB << 8 | T_LSB;

					temperature = -45 + 175.0/(float)(1<<16) * t_dout;
				}
			}
		}
	}

}

uint8_t Altimeter::send8BitWord(uint8_t word)
{
	Wire.beginTransmission(ALTIMETER_ADDRESS);
	Wire.send(word & 0xFF);
	return Wire.endTransmission();
}

uint8_t Altimeter::send8BitWordWithCRC(uint8_t word, uint8_t crc)
{
	Wire.beginTransmission(ALTIMETER_ADDRESS);
	Wire.send(word & 0xFF);
	Wire.send(crc);
	return Wire.endTransmission();
}

uint8_t Altimeter::send16BitWord(uint16_t word)
{
	Wire.beginTransmission(ALTIMETER_ADDRESS);
	Wire.send((word >> 8) & 0xFF);
	Wire.send(word & 0xFF);
	return Wire.endTransmission();
}

uint8_t Altimeter::send16BitWordWithCRC(uint16_t word, uint8_t crc)
{
	Wire.beginTransmission(ALTIMETER_ADDRESS);
	Wire.send((word >> 8) & 0xFF);
	Wire.send(word & 0xFF);
	Wire.send(crc);
	return Wire.endTransmission();
}

bool Altimeter::readBytes(uint8_t* buf, uint8_t len)
{
	Wire.requestFrom(ALTIMETER_ADDRESS, 2);
	if(Wire.available() >= len+1)
	{
		for(int i=0; i<len; i++)
		{
			buf[i] = Wire.read();
		}
		uint8_t crc = Wire.read();
		if(generateCRC8(buf, len) != crc)
		{
			Telemetry::printf(MSG_ERROR, "Altimeter CRC ERROR!\n");
			return false;
		}
		return true;
	}
	return false;
}

uint8_t Altimeter::generateCRC8(const uint8_t *bytes, uint8_t length)
{
	return CRC8.generic(0x31, 0xFF, CRC_FLAG_NOREFLECT, bytes, length);
}
