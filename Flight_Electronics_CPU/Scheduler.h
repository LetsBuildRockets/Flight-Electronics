/*
 * Scheduler.h
 *
 *  Created on: Jan 26, 2019
 *      Author: erics
 */
#include <Arduino.h>
#include <queue>
#include <IntervalTimer.h>

#include "Config.h"
#include "Telemetry.h"

#ifndef SOFTSCHEDULER_H_
#define SOFTSCHEDULER_H_

#define PIT_id 0 // which timer do we want to use. If you change this you also must change the ISR in Scheduler.cpp
typedef void (*ISR)();
enum TASK_PRIORITY
{
	LOW_PRIORITY,
	HIGH_PRIORITY
};

struct Task
{
	ISR fun_ptr;
	uint32_t lastExecuteTime; // in microseconds
	uint32_t interval; // in microseconds
	String name;
};

namespace Scheduler
{
	namespace
	{
		std::queue<Task> taskList_LOW_PRIORITY;
		std::queue<Task> taskList_HIGH_PRIORITY;
		volatile uint8_t highPriortyTaskListSize = 0;
		volatile ISR highPriortyTasks_fast[50];
		volatile int32_t highPriortyInvertvals_fast [50];
		volatile int32_t highPriortyLastExec_fast [50];
		volatile uint8_t highPriortytaskIndex_fast = 0;

		double averageJitter = 0;
		IntervalTimer hwTimer;
		bool hwTimerRunning = false;
	}
	void addTask(TASK_PRIORITY taskPriortiy, ISR fun_ptr, uint32_t interval);
	void addTask(TASK_PRIORITY taskPriortiy, ISR fun_ptr, uint32_t interval, uint32_t delay, String name);
	void init();
	void tickSoft();
	void tickHard();
	void startHwTimer();
	double getAverageJitter();
	int getQueueSize();
};

#endif /* SOFTSCHEDULER_H_ */
