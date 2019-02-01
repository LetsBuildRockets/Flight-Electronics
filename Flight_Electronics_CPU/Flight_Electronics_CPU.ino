#include <Arduino.h>

#include "Config.h"
#include "Analog.h"
#include "RTC.h"
#include "Power.h"
#include "GPS.h"
#include "Telemetry.h"
#include "IMU.h"
#include "Scheduler.h"
#include "Altimeter.h"
#include "Logger.h"

void setup()
{
	DEBUGSERIAL.begin(DEBUG_BAUD_RATE);
	delay(2000);

	DEBUGSERIAL.print("booting....\n");

	RTC::init();
	Telemetry::init();
	Logger::init();

	delay(2000);

    Telemetry::printf(MSG_INFO, "***Built on  %s  at  %s***\n", __DATE__, __TIME__);

	Altimeter::init();
	Power::init();
	Analog::init();
	GPS::init();
	IMU::init();
	Scheduler::init();

	pinMode(PIN_LED, OUTPUT);

	Analog::updateData();

	Scheduler::addTask(HIGH_PRIORITY, Analog::updateData, 50000ul, 0, "Update Analog Data");
	Scheduler::addTask(LOW_PRIORITY, ([]() { digitalWriteFast(PIN_LED, !(digitalReadFast(PIN_LED))); }), 500000ul, 0, "Blink");
	Scheduler::addTask(LOW_PRIORITY, getIMUData, 100000, 0, "IMU update");
	Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "IMU Calibration: %s\n", IMU::getCalibration().c_str()); }), 5000000ul, 0, "IMU print calibration info");
	Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "average Jitter: %.2f us\n", Scheduler::getAverageJitter()); }), 5000000ul, 0, "print average jitter");
	Scheduler::addTask(LOW_PRIORITY, checkBatteryStatus, 10000000ul, 0, "Check Battery State");
	Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "Free mem: %lu kB\n", FreeMem()/1000); }), 5000000ul, 0, "mem ussage");


	Scheduler::startHwTimer();
}

void loop()
{
	Scheduler::tickSoft();
	delayMicroseconds(100);
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
		Telemetry::printf(MSG_ERROR, "Battery voltage CRITICAL! Voltage: %.2f V\n", voltage);
	}
	else if (voltage < 22.2)
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

