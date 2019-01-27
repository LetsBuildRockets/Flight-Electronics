/*
 * CONFIG.h
 *
 *  Created on: Jan 8, 2019
 *      Author: erics
 */



#ifndef CONFIG_H_
#define CONFIG_H_

#define DEBUG 1

#define DEBUGSERIAL Serial
#define DEBUG_BAUD_RATE 115200

#define TELEMETRYSERIAL Serial1
#define TELEMETRY_BAUD_RATE 57600

#define GPSSERIAL Serial3 // PINS 7 and 8
#define GPS_BAUD_RATE 9600

#define PIN_LED 13
#define PIN_PAD_POWER 2


// APRS Information
#define PTT_PIN 31 // Push to talk pin

// Set your callsign and SSID here. Common values for the SSID are
#define S_CALLSIGN      "KB1ZHC"
#define S_CALLSIGN_ID   1   // 11 is usually for balloons
// Destination callsign: APRS (with SSID=0) is usually okay.
#define D_CALLSIGN      "APRS"
#define D_CALLSIGN_ID   0
// Symbol Table: '/' is primary table '\' is secondary table
#define SYMBOL_TABLE '/'
// Primary Table Symbols: /O=balloon, /-=House, /v=Blue Van, />=Red Car
#define SYMBOL_CHAR 'v'

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
