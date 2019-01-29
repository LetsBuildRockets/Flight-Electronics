/*
 * Analog.cpp
 *
 *  Created on: Jan 9, 2019
 *      Author: erics
 */

#include "Analog.h"

void Analog::init()
{
	// TODO: do we need to init anything? Maybe switch to 16bit adc mode?
}
void Analog::updateData()
{
	for (int i = 0; i < NUMBER_OF_ANALOG_PINS; ++i)
	{
		analogVoltage[i] = analogRead(i) * 3.3/1024.0;
		analogScaled[i] = ANALOG_TRANSFER_FUNCTIONS[i](analogVoltage[i]);
	}

}

double Analog::getVoltage(int ChannelNumber)
{
	return analogVoltage[ChannelNumber];
}

double Analog::getScaledData(int ChannelNumber)
{
	return analogScaled[ChannelNumber];
}

