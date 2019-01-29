#include <Arduino.h>

#include "Config.h"
#include "Analog.h"
#include "RTC.h"
#include "Power.h"
#include "GPS.h"
#include "Telemetry.h"
#include "IMU.h"
#include "SoftScheduler.h"
#include "Altimeter.h"

void setup()
{
	Telemetry::init();

	delay(2000);

    Telemetry::printf(MSG_INFO, "***Built on  %s  at  %s***\n", __DATE__, __TIME__);

	Altimeter::init();
	RTC::init();
	Power::init();
	Analog::init();
	GPS::init();
	IMU::init();
	SoftScheduler::init();

	pinMode(PIN_LED, OUTPUT);

	Analog::updateData();

	SoftScheduler::addTask(Analog::updateData, (uint32_t) 50, 0, "Update Analog Data");
	SoftScheduler::addTask(([]() { digitalWriteFast(PIN_LED, !(digitalReadFast(PIN_LED))); }), (uint32_t) 500, 5, "Blink");
	SoftScheduler::addTask(getIMUData, 50, 0, "IMU update");
	SoftScheduler::addTask(([]() { Telemetry::printf(MSG_INFO, "IMU Calibration: %s\n", IMU::getCalibration().c_str()); }), 5000, 0, "IMU print calibration info");
	SoftScheduler::addTask(([]() { Telemetry::printf(MSG_INFO, "average Jitter: %.2f ms\n", SoftScheduler::getAverageJitter()); }), 5000, 100, "print average jitter");
	SoftScheduler::addTask(checkBatteryStatus, (uint32_t) 10000, 200, "Check Battery State");
}

void loop()
{
	SoftScheduler::tickOnce();
}

void getIMUData()
{
	sensors_vec_t eulerPos = IMU::getEuler();
	Telemetry::printf(MSG_IMU, "%.2f %.2f %.2f\n", eulerPos.roll, eulerPos.pitch, eulerPos.heading);
}

void checkBatteryStatus()
{
	float voltage = Power::getBatteryVoltage();
	if (voltage < 18)
	{
		Telemetry::printf(MSG_WARNING, "Battery voltage low! Voltage: %.2f V\n", voltage);
	}
	else
	{
		Telemetry::printf(MSG_INFO, "Battery Voltage: %.2f V\n", voltage);
	}
}

uint32_t FreeMem()
{
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is (approximately) the free, available ram.
    return stackTop - heapTop;
}

