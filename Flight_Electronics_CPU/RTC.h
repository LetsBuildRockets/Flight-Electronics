/*
 * RTC.h
 *
 *  Created on: Jan 12, 2019
 *      Author: erics
 */

#include <Arduino.h>
#include <String.h>
#include <TimeLib.h>

#include "Config.h"

#ifndef RTC_H_
#define RTC_H_

namespace RTC
{
	void init();
	time_t getTime();
	String getTimeString();
};

#endif /* RTC_H_ */
