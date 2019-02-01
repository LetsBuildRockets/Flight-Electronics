/*
 * IMU.h
 *
 *  Created on: Jan 14, 2019
 *      Author: erics
 */


#ifndef IMU_H_
#define IMU_H_

#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>

#include "Config.h"
#include "Telemetry.h"

namespace IMU
{
	namespace
	{
		Adafruit_BNO055 bno = Adafruit_BNO055(55);
	}
	void init();
	imu::Quaternion getQuat();
	sensors_vec_t getEuler();
	String getCalibration();

};

#endif /* IMU_H_ */
