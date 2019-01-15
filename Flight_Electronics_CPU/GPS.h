/*
 * GPS.h
 *
 *  Created on: Jan 13, 2019
 *      Author: erics
 */

#include <Adafruit_GPS.h>
#include "Config.h"

#ifndef GPS_H_
#define GPS_H_

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
