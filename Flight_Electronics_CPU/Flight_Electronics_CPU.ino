#include "Arduino.h"
#include "Config.h"
#include "Analog.h"

Analog analog;
void setup()
{
	pinMode(PIN_LED, OUTPUT);
	analog.updateData();
	Serial.begin(BAUD_RATE);
}

void loop()
{
	digitalWrite(PIN_LED, HIGH);
	delay(50);
	digitalWrite(PIN_LED, LOW);
	delay(950);


	Serial.printf("Battery Voltage: %.2f V\n\r", analog.getBatteryVoltage());

	analog.updateData();
}
