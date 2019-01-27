#include <Arduino.h>
#include <avr/interrupt.h>

#include "Config.h"
#include "Analog.h"
#include "RTC.h"
#include "Power.h"
#include "GPS.h"
#include "Telemetry.h"
#include "IMU.h"
#include "SoftScheduler.h"

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
	SoftScheduler::init();

	pinMode(PIN_LED, OUTPUT);

	Analog::updateData();

	SoftScheduler::addTask(([]() {
		float voltage = Power::getBatteryVoltage();
		if (voltage < 18)
		{
			Telemetry::printf(MSG_WARNING, "Battery Voltage: %.2f V\n", voltage);
		}
		else
		{
			Telemetry::printf(MSG_INFO, "Battery Voltage: %.2f V\n", voltage);
		}
	}), (uint32_t) 10000, "Check Battery State");

	SoftScheduler::addTask(([]() { digitalWriteFast(PIN_LED, !(digitalReadFast(PIN_LED))); }), (uint32_t) 500, "BLINKY");
	SoftScheduler::addTask(([]() { Analog::updateData(); }), (uint32_t) 50, "Update Analog Data");
}

void loop()
{

	//Telemetry::printf(MSG_INFO, "Battery Voltage: %.2f V\n", Power::getBatteryVoltage());
	//Telemetry::printf(MSG_INFO, "Time: %s\n", RTC::getTimeString().c_str());

	//sensors_vec_t eulerPos = IMU::getEuler();
	//Telemetry::printf(MSG_IMU, "%f %f %f\n", eulerPos.roll, eulerPos.pitch, eulerPos.heading);
	//Telemetry::printf(MSG_INFO, IMU::getCalibration().c_str());


	SoftScheduler::tickOnce();
}

