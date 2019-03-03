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

uint32_t sss = 0;
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
	//Power::init();
	//Analog::init();
	//GPS::init();
	//IMU::init();
	Scheduler::init();

	pinMode(PIN_LED, OUTPUT);

	Analog::updateData();

	// TODO: sequencer
	// TODO: Flight Calculator
	// TODO: test reading altitude data. make atimeter interrupt safe.
	// TODO: SD card buffering, and decrease timeout
	// TODO: shift out for digital
	// TODO: AX-12A Servo
	// TODO: GPS
	// TODO: APRS

	Scheduler::addTask(HIGH_PRIORITY, Analog::updateData, 500000lu, 0, "Update Analog Data");
	//Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "avg alt: %.2f m\n", Altimeter::getAltitude()); }), 1000000lu, 0, "get Alt value");
	Scheduler::addTask(LOW_PRIORITY, Altimeter::getNewSample, 1000000lu, 1000000lu, "get Alt sample");
	//Scheduler::addTask(HIGHEST_PRIORITY, ([]() { digitalWriteFast(PIN_LED, !(digitalReadFast(PIN_LED))); }), 1000000lu, 0, "Blink");
	//Scheduler::addTask(LOW_PRIORITY, getIMUData, 100000lu, 0, "IMU update");
	Scheduler::addTask(LOW_PRIORITY, ([]() { digitalWriteFast(PIN_LED, !(digitalReadFast(PIN_LED))); }), 500000lu, 0, "Blink");
	//Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "IMU Calibration: %s\n", IMU::getCalibration().c_str()); }), 5000000lu, 0, "IMU print calibration info");
	Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "average Jitter: %.2f us\n", Scheduler::getAverageJitter()); }), 5000000lu, 0, "print average jitter");
	//Scheduler::addTask(LOW_PRIORITY, checkBatteryStatus, 10000000lu, 0, "Check Battery State");
	Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "Free mem: %lu kB\n", FreeMem()/1000); }), 5000000lu, 0, "mem usage");


	// EXAMPLE... dont actually use this for anything....
	// This will hang the low high and higher priority threads, but shows that the highest priority still runs.
	// Scheduler::addTask(LOW_PRIORITY, ([]() { DEBUGSERIAL.printf("hanging LOW priority now...\n"); DEBUGSERIAL.printf("boop elapsed: %lu, timenow:%lu\n", micros()-sss, micros()); while(1);}), 10000000ul, 0, "HANG_LOW");
	// Scheduler::addTask(HIGH_PRIORITY, ([]() { DEBUGSERIAL.printf("hanging HIGH priority now...\n"); DEBUGSERIAL.printf("boop elapsed: %lu, timenow:%lu\n", micros()-sss, micros()); while(1);}), 20000000ul, 0, "HANG_HIGH");
	// Scheduler::addTask(HIGHER_PRIORITY, ([]() { DEBUGSERIAL.printf("hanging HIGHER priority now...\n"); DEBUGSERIAL.printf("boop elapsed: %lu, timenow:%lu\n", micros()-sss, micros()); while(1);}), 30000000ul, 0, "HANG_HIGHER");

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

