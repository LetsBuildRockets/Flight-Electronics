/*
 * Analog.h
 *
 *  Created on: Jan 9, 2019
 *      Author: erics
 */


#ifndef ANALOG_H_
#define ANALOG_H_

#include "Arduino.h"
#include "Config.h"

class Analog
{
private:
	int analogRaw[NUMBER_OF_ANALOG_PINS] = { 0 };
	double analogScaled[NUMBER_OF_ANALOG_PINS] = { 0 };
public:
	double getScaledData(int ChannelNumber);
	int getRawData(int ChannelNumber);
	void updateData();
	double getBatteryVoltage();
};



#endif /* ANALOG_H_ */
