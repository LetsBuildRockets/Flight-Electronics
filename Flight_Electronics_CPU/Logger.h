/*
 * Logger.h
 *
 *  Created on: Jan 29, 2019
 *      Author: erics
 */

#include <Arduino.h>
#include  <SdFAT.h>

#include "Config.h"
#include "Telemetry.h"

#define USE_SDIO 1

#ifndef LOGGER_H_
#define LOGGER_H_

namespace Logger
{
	namespace
	{
		SdFatSdio sd;
	}
	void init();
};

#endif /* LOGGER_H_ */
