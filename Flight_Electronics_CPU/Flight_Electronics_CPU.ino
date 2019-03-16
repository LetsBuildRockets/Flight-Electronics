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
#include "Sequencer.h"
#include "aprs.h"



uint32_t sss = 0;
Sequencer *sequencerT;
GPS *gps;

void setup()
{
	//#error This program uses enables the APRS transmitter, make sure you have a 50 ohm load connected. Comment this out to continue...

	DEBUGSERIAL.begin(DEBUG_BAUD_RATE);
	delay(2000);

	// ensure we aren't driving the lookback pin
	pinMode(23, INPUT);
	aprs_setup(50, PTT_PIN, 500, 200, 50);

	DEBUGSERIAL.print("booting....\n");

	RTC::init();
	Telemetry::init();
	Logger::init();

	delay(2000);

    Telemetry::printf(MSG_INFO, "***Built on  %s  at  %s***\n", __DATE__, __TIME__);

	Altimeter::init();
	Power::init();
	Analog::init();
	IMU::init();
	Scheduler::init();

	sequencerT = new Sequencer(-30000);
	Serial.println("got this far...0");
	sequencerT->addSeqTask(-20000, 0, ([](){Telemetry::printf(MSG_INFO, "bleh");}), "bleh");
	sequencerT->addSeqTask(-10000, 10, ([](){Telemetry::printf(MSG_INFO, "bleh2");}), "bleh2");
	sequencerT->addSeqTask(0, 10, ([](){Telemetry::printf(MSG_INFO, "bleh3");}), "bleh3");
	sequencerT->addSeqTask(100, 10, ([](){Telemetry::printf(MSG_INFO, "liftoff");}), "fucking liftoff");
	Serial.println("got this far...1");
	sequencerT->printTaskList();


	gps = new GPS(&GPSSERIAL,false);
	gps->startSerial(GPS_BAUD_RATE);
	delay(1000);
	gps->setSentencesToReceive(OUTPUT_RMC_GGA);
	gps->setUpdateRate(UPDATE_RATE_5000);


	pinMode(PIN_LED, OUTPUT);

	Analog::updateData();

	// TODO: sequencer
	// TODO: Flight Calculator
	// TODO: make altimeter interrupt safe.
	// TODO: SD card buffering, and decrease timeout
	// TODO: AX-12A Servo

	// TODO: Um using high priority tasks breaks everything... don't do it until its fixed. thanks bye.

	//Scheduler::addTask(LOW_PRIORITY, Analog::updateData, 500000lu, 0, "Update Analog Data");
	Scheduler::addTask(LOW_PRIORITY, getGPS, 100000lu, 0, "Poll GPS");
	//Scheduler::addTask(LOW_PRIORITY, printGPS, 100000lu, 0, "Print GPS");
	Scheduler::addTask(LOW_PRIORITY, ([]() { broadcastLocation("um. is this think working?"); }), 10000000ul, 0000000ul, "Queue an APRS broadcast");
	//Scheduler::addTask(LOW_PRIORITY, ([]() {sequencerT->tick();}), 1000lu, 0, "Tick Seqeuncer");
	//Scheduler::addTask(LOW_PRIORITY, ([]() {sequencerT->start();}), 0, 10000000ul, "Start Seqeuncer");
	Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "avg alt: %.2f m\n", Altimeter::getAltitude()); }), 1000000lu, 0, "get Alt value");
	Scheduler::addTask(LOW_PRIORITY, Altimeter::getNewSample, 100000lu, 1000000lu, "get Alt sample");
	Scheduler::addTask(LOW_PRIORITY, getIMUData, 100000lu, 0, "IMU update");
	Scheduler::addTask(LOW_PRIORITY, ([]() { digitalWriteFast(PIN_LED, !(digitalReadFast(PIN_LED))); }), 500000lu, 0, "Blink");
	Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "IMU Calibration: %s\n", IMU::getCalibration().c_str()); }), 5000000lu, 0, "IMU print calibration info");
	Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "average Jitter: %.2f us\n", Scheduler::getAverageJitter()); }), 5000000lu, 0, "print average jitter");
	Scheduler::addTask(LOW_PRIORITY, checkBatteryStatus, 10000000lu, 0, "Check Battery State");
	Scheduler::addTask(LOW_PRIORITY, ([]() { Telemetry::printf(MSG_INFO, "Free mem: %lu kB\n", FreeMem()/1000); }), 5000000lu, 0, "mem usage");


	// EXAMPLE... dont actually use this for anything....
	// This will hang the low high and higher priority threads, but shows that the highest priority still runs.
	// Scheduler::addTask(LOW_PRIORITY, ([]() { DEBUGSERIAL.printf("hanging LOW priority now...\n"); DEBUGSERIAL.printf("boop elapsed: %lu, timenow:%lu\n", micros()-sss, micros()); while(1);}), 10000000ul, 0, "HANG_LOW");
	// Scheduler::addTask(HIGH_PRIORITY, ([]() { DEBUGSERIAL.printf("hanging HIGH priority now...\n"); DEBUGSERIAL.printf("boop elapsed: %lu, timenow:%lu\n", micros()-sss, micros()); while(1);}), 20000000ul, 0, "HANG_HIGH");
	// Scheduler::addTask(HIGHER_PRIORITY, ([]() { DEBUGSERIAL.printf("hanging HIGHER priority now...\n"); DEBUGSERIAL.printf("boop elapsed: %lu, timenow:%lu\n", micros()-sss, micros()); while(1);}), 30000000ul, 0, "HANG_HIGHER");

	//Scheduler::startHwTimer();

}

void loop()
{
	Scheduler::tickSoft();
	delayMicroseconds(100);
}

void getGPS()
{
	gps->poll();
	if (gps->sentenceAvailable())
	{
		gps->parseSentence();
	}
}

void printGPS()
{
	int rtn = -1;
	if (gps->sentenceAvailable())
	{
		DEBUGSERIAL.printf("GPS sent: %s", gps->getLastSentence());
		rtn = gps->parseSentence();
	}

	if(rtn!=-1) Telemetry::printf(MSG_ERROR, "GPS returned: %i, fix: %i, quality: %i, sats: %i\n", rtn, gps->fix, gps->fixquality, gps->satellites);
	if (gps->newValuesSinceDataRead())
	{
		gps->dataRead();
		DEBUGSERIAL.printf("Location: %f, %f altitude %f\n\r", gps->latitude, gps->longitude, gps->altitude);
	}
}

struct PathAddress addresses[] = {
  {(char *)D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
  {(char *)S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign
  {(char *)NULL, 0}, // Digi1 (first digi in the chain)
  {(char *)NULL, 0}  // Digi2 (second digi in the chain)
};

void broadcastLocation(const char *comment)
{
	if(!gps->fix) Telemetry::printf(MSG_WARNING, "No GPS Fix\n");
	Telemetry::printf(MSG_GPS, "%d,%u,%u,%u,%f,%f,%f,%u,%f,%u\n", gps->fix, gps->day, gps->hour, gps->minute ,gps->latitude, gps->longitude,gps->altitude,gps->heading,gps->speed,gps->satellites);
	// If above 5000 feet switch to a single hop path
	int nAddresses;
	addresses[2].callsign = "WIDE2";
	addresses[2].ssid = 1;

	// For debugging print out the path
	/*Serial.print("APRS(");
	Serial.print(nAddresses);
	Serial.print("): ");
	for (int i=0; i < nAddresses; i++) {
	Serial.print(addresses[i].callsign);
	Serial.print('-');
	Serial.print(addresses[i].ssid);
	if (i < nAddresses-1)
		Serial.print(',');
	}
	Serial.print(' ');
	Serial.print(SYMBOL_TABLE);
	Serial.print(SYMBOL_CHAR);
	Serial.println();*/

	// Send the packet
	aprs_send(addresses, nAddresses
		,gps->day, gps->hour, gps->minute
		,gps->latitude, gps->longitude // degrees
		,gps->altitude // meters
		,gps->heading
		,gps->speed
		,SYMBOL_TABLE
		,SYMBOL_CHAR
		,comment);
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

