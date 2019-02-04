/*
 * Analog.h
 *
 *  Created on: Jan 9, 2019
 *      Author: erics
 */


#ifndef ANALOG_H_
#define ANALOG_H_

#include <Arduino.h>

#include "Config.h"

#define ADC_RESOLUTION 16
#define ADC_RANGE (float)(1ul << ADC_RESOLUTION)

#define ADC_AVERAGING_COUNT 4

namespace Analog
{
	namespace
	{
		volatile uint16_t analogADCValue[NUMBER_OF_ANALOG_PINS] = { 0 };
	}
	void init();
	float getScaledData(uint8_t channelNumber);
	float getVoltage(uint8_t channelNumber);
	uint16_t getADCValue(uint8_t channelNumber);
	void updateData();
};



#endif /* ANALOG_H_ */
