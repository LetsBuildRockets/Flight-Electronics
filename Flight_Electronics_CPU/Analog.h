/*
 * Analog.h
 *
 *  Created on: Jan 9, 2019
 *      Author: erics
 */

#include <Arduino.h>

#include "Config.h"

#ifndef ANALOG_H_
#define ANALOG_H_

namespace Analog
{
	namespace
	{
		volatile double analogVoltage[NUMBER_OF_ANALOG_PINS] = { 0 };
		volatile double analogScaled[NUMBER_OF_ANALOG_PINS] = { 0 };
	}
	void init();
	double getScaledData(int ChannelNumber);
	double getVoltage(int ChannelNumber);
	void updateData();
};



#endif /* ANALOG_H_ */
