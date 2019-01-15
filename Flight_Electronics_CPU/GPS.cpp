/*
 * GPS.cpp
 *
 *  Created on: Jan 13, 2019
 *      Author: erics
 */

#include "GPS.h"

void GPS::init()
{
	_GPS.begin(GPS_BAUD_RATE);
	_GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // TODO: We should probably switch this to PMTK_SET_NMEA_OUTPUT_RMCONLY, after everything is working
	// Set update rate
	_GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
	_GPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ);
	// Get antenna status
	_GPS.sendCommand(PGCMD_ANTENNA);
	// Get GPS module firmware version
	_GPS.sendCommand(PMTK_Q_RELEASE);

	GPSIntervalTimer.begin(refreshGPS, 10000L);
}

void GPS::refreshGPS()
{
	if (_GPS.newNMEAreceived()) {
	    // a tricky thing here is if we print the NMEA sentence, or data
	    // we end up not listening and catching other sentences!
	    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
		String newNME = _GPS.lastNMEA();
	    Serial.println(newNME);   // this also sets the newNMEAreceived() flag to false

	    if (!_GPS.parse(const_cast<char*> (newNME.c_str())))   // this also sets the newNMEAreceived() flag to false
	      return;  // we can fail to parse a sentence in which case we should just wait for another
	  }
}


