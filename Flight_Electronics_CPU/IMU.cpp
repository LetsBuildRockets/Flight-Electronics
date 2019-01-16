/*
 * IMU.cpp
 *
 *  Created on: Jan 14, 2019
 *      Author: erics
 */

#include "IMU.h"

void IMU::init()
{
	Wire.setSCL(33);
	Wire.setSDA(34);
	if(!bno.begin())
	{
		Telemetry::printf(MSG_ERROR, "IMU NOT CONNECTED!\n");
	}
	else
	{
		Telemetry::printf(MSG_INFO, "IMU is connected\n");
	}
	bno.setExtCrystalUse(true);
}

imu::Quaternion IMU::getQuat()
{
	return bno.getQuat();
}

sensors_vec_t IMU::getEuler()
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
