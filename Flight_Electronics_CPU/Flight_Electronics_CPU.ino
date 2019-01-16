#include <Arduino.h>
#include <avr/interrupt.h>

#include "Config.h"
#include "Analog.h"
#include "RTC.h"
#include "Power.h"
#include "GPS.h"
#include "Telemetry.h"

void setup()
{
    Serial.printf("***Built on  %s  at  %s***\n", __DATE__, __TIME__);

	RTC::init();
	Power::init();
	Analog::init();
	GPS::init();
	Telemetry::init();

	Analog::updateData();
}

void loop()
{
	digitalWrite(PIN_LED, HIGH);
	delay(50);
	digitalWrite(PIN_LED, LOW);
	delay(1950);


	Telemetry::printf(MSG_INFO, "Battery Voltage: %.2f V\n", Power::getBatteryVoltage());
	Telemetry::printf(MSG_INFO, "Time: %s\n", RTC::getTimeString().c_str());

	Analog::updateData();
}
