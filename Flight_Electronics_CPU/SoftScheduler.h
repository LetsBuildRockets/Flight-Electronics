/*
 * SoftScheduler.h
 *
 *  Created on: Jan 26, 2019
 *      Author: erics
 */
#include <Arduino.h>
#include <deque>

#include "Config.h"
#include "Telemetry.h"

#ifndef SOFTSCHEDULER_H_
#define SOFTSCHEDULER_H_

struct Task
{
	 void (*fun_ptr)(void);
	 uint32_t lastExecuteTime; // in milliseconds
	 uint16_t interval; // in milliseconds
	 String name;
};

namespace SoftScheduler
{
	namespace
	{
		std::deque<Task> taskList;
		double averageJitter = 0;
	}
	void addTask(void (*fun_ptr)(void), uint32_t interval);
	void addTask(void (*fun_ptr)(void), uint32_t interval, String name);
	void init();
	void tickOnce();
	double getAverageJitter();
};

#endif /* SOFTSCHEDULER_H_ */
