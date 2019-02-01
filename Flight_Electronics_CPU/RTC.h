/*
 * RTC.h
 *
 *  Created on: Jan 12, 2019
 *      Author: erics
 */


#ifndef RTC_H_
#define RTC_H_

#include <Arduino.h>
#include <String.h>
#include <TimeLib.h>

#include "Config.h"

namespace RTC
{
	void init();
	time_t getTime();
	String getTimeString(bool useDelimiter = true);
};

#endif /* RTC_H_ */
