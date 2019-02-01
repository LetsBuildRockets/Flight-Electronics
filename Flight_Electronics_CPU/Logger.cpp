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

		filename = RTC::getTimeString(false)+".log";

		DEBUGSERIAL.println("Trying to open SD card");
		if (!logFile.open(filename.c_str(), O_RDWR | O_CREAT)) {
			Telemetry::printf(MSG_ERROR, "couldn't create %s file\n",filename.c_str());
		}
		logFile.flush();
		logFile.close();

		Telemetry::printf(MSG_INFO, "Connected SD card!\nCard Size: %.0f MB\n", sd.card()->cardSize()*0.000512);

		Logger::printf("headers and shit\n");
	}

	else
	{
		DEBUGSERIAL.println("SD ERROR!!!!!!");
		DEBUGSERIAL.flush();
		sd.initErrorHalt();
	}
}

void Logger::printf(const char * format, ...)
{
	char *buffer = (char *)malloc(1024*sizeof(char));
	va_list vargs;
	va_start(vargs, format);
	vsprintf(buffer, format, vargs);
	if(logFile.open(filename.c_str(), O_WRITE | O_APPEND))
	{
		logFile.print(buffer);
		logFile.flush();
	}
	else
	{
		Telemetry::printf(MSG_ERROR, "Could not log to SD card!\n");
	}
	logFile.close();
	va_end(vargs);
	free(buffer);
}

void Logger::log(const char * buf, uint8_t bufsize)
{
	if(logFile.open(filename.c_str(), O_WRITE | O_APPEND))
	{
		logFile.print(buf);
		logFile.flush();
	}
	else
	{
		Telemetry::printf(MSG_ERROR, "Could not log to SD card!\n");
	}
	logFile.close();
}
