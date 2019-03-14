/*
 * IMU.cpp
 *
 *  Created on: Jan 14, 2019
 *      Author: erics
 */

#include "IMU.h"

void IMU::init()
{
	Wire.setSCL(I2C_PIN_SCL);
	Wire.setSDA(I2C_PIN_SDA);
	Wire.setClock(I2C_CLOCK_SPEED);
	Wire.setTimeout(I2C_TIMEOUT);
	if(!bno.begin())
	{
		Telemetry::printf(MSG_ERROR, "IMU NOT CONNECTED!\n");
		imuRunning = false;
	}
	else
	{
		Telemetry::printf(MSG_INFO, "IMU is connected\n");
		imuRunning = true;
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
	uint8_t sys = 0, gyro = 0, accel = 0, mag = 0;
	char buffer[128];
	if(imuRunning)
		bno.getCalibration(&sys, &gyro, &accel, &mag);
	sprintf(buffer, "sys: %d, gyro: %d, accel: %d, mag: %d", sys, gyro, accel, mag);
	return String(buffer);
}
