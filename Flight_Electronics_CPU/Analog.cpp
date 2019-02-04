/*
 * Analog.cpp
 *
 *  Created on: Jan 9, 2019
 *      Author: erics
 */

#include "Analog.h"

void Analog::init()
{
	analogReadResolution(ADC_RESOLUTION);
	analogReadAveraging(ADC_AVERAGING_COUNT);
}
void Analog::updateData()
{
	for (int i = 0; i < NUMBER_OF_ANALOG_PINS; ++i)
	{
		float newValue = analogRead(i);
		__disable_irq();
		analogADCValue[i] = newValue;
		__enable_irq();
	}

}

float Analog::getVoltage(uint8_t channelNumber)
{
	__disable_irq();
	float _analogVoltage=analogADCValue[channelNumber]  * 3.3/ADC_RANGE;
	__enable_irq();
	return _analogVoltage;
}

float Analog::getScaledData(uint8_t channelNumber)
{
	__disable_irq();
	float _analogScaled = ANALOG_TRANSFER_FUNCTIONS[channelNumber](analogADCValue[channelNumber]  * 3.3/ADC_RANGE);
	__enable_irq();
	return _analogScaled;
}

