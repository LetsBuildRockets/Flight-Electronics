/*
 * Power.cpp
 *
 *  Created on: Jan 12, 2019
 *      Author: erics
 */

#include "Power.h"

void Power::init() {
	pinMode(PIN_PAD_POWER, OUTPUT);
	digitalWrite(PIN_PAD_POWER, 0);
}

double Power::getBatteryVoltage()
{
	return Analog::getScaledData(8);
}

void Power::forceSwitchToInternalPower(bool bl)
{
	digitalWrite(PIN_PAD_POWER, bl);
}

