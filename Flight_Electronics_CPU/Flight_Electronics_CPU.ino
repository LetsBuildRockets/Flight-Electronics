#include <Arduino.h>
#include <avr/interrupt.h>

#include "Config.h"
#include "Analog.h"
#include "RTC.h"
#include "Power.h"
#include "GPS.h"
#include "Telemetry.h"
#include "IMU.h"

void setup()
{
	Telemetry::init();

	delay(2000);

    Telemetry::printf(MSG_INFO, "***Built on  %s  at  %s***\n", __DATE__, __TIME__);


	RTC::init();
	Power::init();
	Analog::init();
	GPS::init();
	IMU::init();

	pinMode(PIN_LED, OUTPUT);

	Analog::updateData();
}

void loop()
{
	digitalWrite(PIN_LED, HIGH);
	delay(50);
	digitalWrite(PIN_LED, LOW);
	delay(50);


	Telemetry::printf(MSG_INFO, "Battery Voltage: %.2f V\n", Power::getBatteryVoltage());
	Telemetry::printf(MSG_INFO, "Time: %s\n", RTC::getTimeString().c_str());

	sensors_vec_t eulerPos = IMU::getEuler();
	Telemetry::printf(MSG_IMU, "%f %f %f\n", eulerPos.roll, eulerPos.pitch, eulerPos.heading);
	Telemetry::printf(MSG_INFO, IMU::getCalibration().c_str());

	Analog::updateData();
}
