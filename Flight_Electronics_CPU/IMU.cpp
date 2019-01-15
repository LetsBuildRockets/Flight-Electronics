/*
 * IMU.cpp
 *
 *  Created on: Jan 14, 2019
 *      Author: erics
 */

#include "IMU.h"

void IMU::init()
{
	bno.getSensor(&sensorIMU);
	bno.setExtCrystalUse(true);
}

Quaternion IMU::getQuat()
{
	return bno.getQuat();
}

Euler IMU::getEuler()
{
	 sensors_event_t event;
	 bno.getEvent(&event);

	 return event.orientation;
}

String IMU::getCalibration()
{
	uint8_t sys, gyro, accel, mag = 0;
	char buffer[128];
	bno.getCalibration(&sys, &gyro, &accel, &mag);
	sprintf(buffer, "sys: %d, gyro: %d, accel: %d, mag: %d", sys, gyro, accel, mag);
	return String(buffer);
}
