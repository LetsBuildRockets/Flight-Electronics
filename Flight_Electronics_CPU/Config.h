/*
 * CONFIG.h
 *
 *  Created on: Jan 8, 2019
 *      Author: erics
 */



#ifndef CONFIG_H_
#define CONFIG_H_

#define TELEMETRYSERIAL Serial
#define TELEMETRY_BAUD_RATE 57600

#define GPSSERIAL Serial3 // PINS 7 and 8
#define GPS_BAUD_RATE 9600

#define PIN_LED 13
#define PIN_PAD_POWER 2

#define NUMBER_OF_ANALOG_PINS 9

typedef double(*TransferFunctionPointer)(int);
const TransferFunctionPointer ANALOG_TRANSFER_FUNCTIONS[NUMBER_OF_ANALOG_PINS] =
{
		[](int raw) -> double { return raw*1; }, // 0
		[](int raw) -> double { return raw*2; }, // 1
		[](int raw) -> double { return raw*3; }, // 2
		[](int raw) -> double { return raw*4; }, // 3
		[](int raw) -> double { return raw*5; }, // 4
		[](int raw) -> double { return raw*6; }, // 5
		[](int raw) -> double { return raw*7; }, // 6
		[](int raw) -> double { return raw*8; }, // 7
		[](int raw) -> double { return 3.3*raw/1024.0*(15+2.2)/(2.2); } // 8 - Battery Voltage
};

enum TELEMETRY_MSG_TYPE
{
	MSG_IGNORE,
	MSG_ERROR,
	MSG_WARNING,
	MSG_INFO,
	MSG_GPS,
	MSG_IMU,
	MSG_POWER
};

#endif /* CONFIG_H_ */
