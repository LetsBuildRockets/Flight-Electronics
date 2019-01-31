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
		double newVoltage = analogRead(i) * 3.3/1024.0;
		__disable_irq();
		analogVoltage[i] = newVoltage;
		analogScaled[i] = ANALOG_TRANSFER_FUNCTIONS[i](analogVoltage[i]);
		__enable_irq();
	}

}

double Analog::getVoltage(int ChannelNumber)
{
	__disable_irq();
	double _analogVoltage=analogVoltage[ChannelNumber];
	__enable_irq();
	return _analogVoltage;
}

double Analog::getScaledData(int ChannelNumber)
{
	__disable_irq();
	double _analogScaled=analogScaled[ChannelNumber];
	__enable_irq();
	return _analogScaled;
}

