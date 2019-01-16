/*
 * TELEMETRY.h
 *
 *  Created on: Jan 15, 2019
 *      Author: erics
 */
#include <Arduino.h>
#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "Config.h"

#ifndef TELEMETRY_H_
#define TELEMETRY_H_

namespace Telemetry
{
	void init();
	void sendMSG(TELEMETRY_MSG_TYPE messageType, std::vector<uint8_t> buffer);
	void printf(TELEMETRY_MSG_TYPE messageType, char * format, ...);
};

#endif /* TELEMETRY_H_ */
