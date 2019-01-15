#include <Arduino.h>
#include <avr/interrupt.h>

#include "Config.h"
#include "Analog.h"
#include "RTC.h"
#include "Power.h"
#include "GPS.h"

void setup()
{
    Serial.printf("***Built on  %s  at  %s***\n", __DATE__, __TIME__);

	RTC::init();
	Power::init();
	Analog::init();
	GPS::init();

	Analog::updateData();
	Serial.begin(SERIAL_BAUD_RATE);
}

void loop()
{
	digitalWrite(PIN_LED, HIGH);
	delay(50);
	digitalWrite(PIN_LED, LOW);
	delay(950);


	Serial.printf("Battery Voltage: %.2f V\n", Power::getBatteryVoltage());
	Serial.printf("Time: %s\n", RTC::getTimeString().c_str());

	Analog::updateData();
}
