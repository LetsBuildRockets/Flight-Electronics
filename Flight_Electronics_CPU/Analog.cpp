/*
 * Analog.cpp
 *
 *  Created on: Jan 9, 2019
 *      Author: erics
 */

#include "Analog.h"

void Analog::updateData()
{
	for (int i = 0; i < NUMBER_OF_ANALOG_PINS; ++i)
	{
		analogRaw[i] = analogRead(i);
		analogScaled[i] = ANALOG_TRANSFER_FUNCTIONS[i](analogRaw[i]);
	}

}

int Analog::getRawData(int ChannelNumber)
{
	return analogRaw[ChannelNumber];
}

double Analog::getScaledData(int ChannelNumber)
{
	return analogScaled[ChannelNumber];
}

double Analog::getBatteryVoltage()
{
	return getScaledData(8);
}
