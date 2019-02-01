/*
 * Logger.h
 *
 *  Created on: Jan 29, 2019
 *      Author: erics
 */



#ifndef LOGGER_H_
#define LOGGER_H_

#define USE_SDIO 1
#define SD_ERROR_LIMIT 2

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
		bool fileOpen = false;
		uint8_t numberOfSDCardError = 0;
	}
	void init();
	void printf(const char * format, ...);
	void log(const char * buf, uint8_t bufsize);
};

#endif /* LOGGER_H_ */
