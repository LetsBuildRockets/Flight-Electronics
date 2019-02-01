/*
 * GPS.h
 *
 *  Created on: Jan 13, 2019
 *      Author: erics
 */

#ifndef GPS_H_
#define GPS_H_

#include <Arduino.h>
#include <Adafruit_GPS.h>

#include "Config.h"

namespace GPS
{
	namespace
	{
		Adafruit_GPS _GPS(&GPSSERIAL);
		IntervalTimer GPSIntervalTimer;
	}
	void refreshGPS();
	void init();
};

#endif /* GPS_H_ */
