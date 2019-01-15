/*
 * IMU.h
 *
 *  Created on: Jan 14, 2019
 *      Author: erics
 */

#include "Adafruit_BNO055.h"
#include "Arduino.h"
#include "Adafruit_Sensor.h"
#include "utility/imumaths.h"
#ifndef IMU_H_
#define IMU_H_

typedef imu::Quaternion Quaternion;
typedef imu::Vector<3> Euler;

namespace IMU
{
	namespace
	{
		Adafruit_BNO055 bno = Adafruit_BNO055(55);
		sensor_t sensorIMU;
	}
	void init();
	Quaternion getQuat();
	Euler getEuler();
	String getCalibration();

};

#endif /* IMU_H_ */
