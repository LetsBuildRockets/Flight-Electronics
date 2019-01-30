/*
 * Logger.cpp
 *
 *  Created on: Jan 29, 2019
 *      Author: erics
 */

#include "Logger.h"

void Logger::init()
{
	if (sd.begin())
	{
		Telemetry::printf(MSG_INFO, "Connected SD card!\nCard Size: %.0f MB\n", sd.card()->cardSize()*0.000512);
	}
	else
	{
		sd.initErrorHalt();
		Telemetry::printf(MSG_ERROR, "cannot connect to SD card!\n");
	}
}
