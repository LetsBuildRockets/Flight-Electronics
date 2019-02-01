/*
 * TELEMETRY.cpp
 *
 *  Created on: Jan 15, 2019
 *      Author: erics
 */

#include "Telemetry.h"

void Telemetry::init()
{
	TELEMETRYSERIAL.begin(TELEMETRY_BAUD_RATE);
}

void Telemetry::sendMSG(TELEMETRY_MSG_TYPE messageType, uint8_t *buffer, size_t bufferLength, bool logToSDCard)
{
	bufferLength += 2;
	uint8_t _buffer[bufferLength];
	memcpy(_buffer + 1, buffer, bufferLength);
	_buffer[0] = (uint8_t) messageType;
	_buffer[bufferLength - 1] = '\0';
	TELEMETRYSERIAL.flush();
	TELEMETRYSERIAL.write(_buffer, bufferLength);
	if(logToSDCard) Logger::log((const char*)_buffer, bufferLength);
#if DEBUG
	DEBUGSERIAL.printf("Telemetry msg: %s\n", _buffer);
#endif
}

void Telemetry::printf(TELEMETRY_MSG_TYPE messageType, const char * format, ...)
{
	char *buffer = (char *)malloc(128*sizeof(char));
	va_list vargs;
	va_start(vargs, format);
	vsprintf(buffer, format, vargs);
	Telemetry::sendMSG(messageType, (uint8_t*)buffer, (size_t) String(buffer).length(), true);
	va_end(vargs);
	free(buffer);
}

void Telemetry::printfNOLOG(TELEMETRY_MSG_TYPE messageType, const char * format, ...)
{
	char *buffer = (char *)malloc(128*sizeof(char));
	va_list vargs;
	va_start(vargs, format);
	vsprintf(buffer, format, vargs);
	Telemetry::sendMSG(messageType, (uint8_t*)buffer, (size_t) String(buffer).length(), false);
	va_end(vargs);
	free(buffer);
}
