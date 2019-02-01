/*
 * Altimeter.h
 *
 *  Created on: Jan 28, 2019
 *      Author: erics
 */


#ifndef ALTIMETER_H_
#define ALTIMETER_H_

#include <Arduino.h>
#include <Wire.h>
#include <FastCRC.h>

#include "Config.h"
#include "Telemetry.h"

#define ALTIMITER_MODE_LOW_POWER 0x401A
#define ALTIMETER_MODE_NORMAL 0x48A3
#define ALTIMETER_MODE_LOW_NOISE 0x5059
#define ALTIMETER_MODE_ULTRA_LOW_NOISE 0x58E0

namespace Altimeter
{
	namespace
	{
		uint32_t p_dout;
		uint16_t t_dout;
		float pressure;
		float velocity;;
		float temperature;
		uint16_t cn[4] = {0};
		FastCRC8 CRC8;
	}
	void init();
	float getVelocity();
	float getAlittude();
	uint8_t send8BitWord(uint8_t word);
	uint8_t send8BitWordWithCRC(uint8_t word, uint8_t crc);
	uint8_t send16BitWord(uint16_t word);
	uint8_t send16BitWordWithCRC(uint16_t word, uint8_t crc);
	uint8_t generateCRC8(const uint8_t *bytes, uint8_t length);
	uint8_t generate8bitWordCRC8(uint8_t);
	uint8_t generate16bitWordCRC8(uint16_t);
	bool updateOPT();
	bool readBytes(uint8_t* buf, uint8_t len);
	void getNewSample();
};

#endif /* ALTIMETER_H_ */
