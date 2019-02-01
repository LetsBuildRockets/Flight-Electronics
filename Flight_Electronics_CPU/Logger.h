/*
 * Logger.h
 *
 *  Created on: Jan 29, 2019
 *      Author: erics
 */



#ifndef LOGGER_H_
#define LOGGER_H_

#define USE_SDIO 1

#include <Arduino.h>
#include  <SdFAT.h>

#include "Config.h"
#include "Telemetry.h"
#include "RTC.h"

namespace Logger
{
	namespace
	{
		SdFatSdio sd;
		SdFile logFile;
		String filename;
	}
	void init();
	void printf(const char * format, ...);
	void log(const char * buf, uint8_t bufsize);
};

#endif /* LOGGER_H_ */
