/*
 * Power.h
 *
 *  Created on: Jan 12, 2019
 *      Author: erics
 */
#include <Arduino.h>

#include "Config.h"
#include "Analog.h"

#ifndef POWER_H_
#define POWER_H_

namespace Power
{
	void init();
	double getBatteryVoltage();
	void forceSwitchToInternalPower(bool bl);
	bool isOnInternalPower();
};

#endif /* POWER_H_ */
