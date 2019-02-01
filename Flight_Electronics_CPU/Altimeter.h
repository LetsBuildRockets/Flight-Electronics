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

namespace Altimeter
{
	namespace
	{
		float pressure;
		float velocity;;
		FastCRC8 CRC8;
	}
	void init();
	float getVelocity();
	float getAlittude();
	void send16BitWord(uint16_t word);
	uint8_t generateCRC8(const uint8_t *bytes, uint8_t length);
	uint8_t generate8bitWordCRC8(uint8_t);
	uint8_t generate16bitWordCRC8(uint16_t);
	void readBytes(uint8_t* buf, uint8_t len);
};

#endif /* ALTIMETER_H_ */
