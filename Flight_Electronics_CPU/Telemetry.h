/*
 * TELEMETRY.h
 *
 *  Created on: Jan 15, 2019
 *      Author: erics
 */

#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include <Arduino.h>
#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "Config.h"
#include "Logger.h"


namespace Telemetry
{
	void init();
	void sendMSG(TELEMETRY_MSG_TYPE messageType, uint8_t *buffer, size_t bufferLength, bool logToSDCard = true);
	void printf(TELEMETRY_MSG_TYPE messageType, const char * format, ...);
	void printfNOLOG(TELEMETRY_MSG_TYPE messageType, const char * format, ...);
};

#endif /* TELEMETRY_H_ */
