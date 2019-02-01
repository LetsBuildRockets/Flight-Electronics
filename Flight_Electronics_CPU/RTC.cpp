/*
 * RTC.cpp
 *
 *  Created on: Jan 12, 2019
 *      Author: erics
 */

#include "RTC.h"

void RTC::init() {
	setSyncProvider(getTime);
}

time_t RTC::getTime()
{
	return Teensy3Clock.get();
}

String RTC::getTimeString(bool useDelimeter)
{
    char buffer[100];
    if(useDelimeter)
    	sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d", year(), day(), month(), hour(), minute(), second());
    else
    	sprintf(buffer, "%04d%02d%02dT%02d%02d%02d", year(), day(), month(), hour(), minute(), second());
    return String(buffer);
}
