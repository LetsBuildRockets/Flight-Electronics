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
		int analogRaw[NUMBER_OF_ANALOG_PINS] = { 0 };
		double analogScaled[NUMBER_OF_ANALOG_PINS] = { 0 };
	}
	void init();
	double getScaledData(int ChannelNumber);
	int getRawData(int ChannelNumber);
	void updateData();
};



#endif /* ANALOG_H_ */
