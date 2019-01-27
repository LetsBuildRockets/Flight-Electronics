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

void Telemetry::sendMSG(TELEMETRY_MSG_TYPE messageType, std::vector<uint8_t> buffer)
{
	int bufferLength = (buffer.size() + 2);
	int buffersize = bufferLength * sizeof(uint8_t);
	uint8_t _buffer[buffersize];
	memcpy(_buffer + 1, buffer.data(), buffer.size() * sizeof(uint8_t));
	_buffer[0] = (uint8_t) messageType;
	_buffer[bufferLength - 1] = '\0';
	TELEMETRYSERIAL.write(_buffer, buffersize);
#if DEBUG
	char __buffer[buffersize + 20];
	strcpy(__buffer, "Telemetry msg: ");
	strcpy(__buffer, (const char*)(_buffer));
	DEBUGSERIAL.println(__buffer);
#endif
}

void Telemetry::printf(TELEMETRY_MSG_TYPE messageType, const char * format, ...)
{
	char *buffer = (char *)malloc(128*sizeof(char));
	va_list vargs;
	va_start(vargs, format);
	vsprintf(buffer, format, vargs);
	String str(buffer);
	std::vector<uint8_t> data(&buffer[0], &buffer[str.length()]);
	Telemetry::sendMSG(messageType, data);
	va_end(vargs);
}
