/*
 * SoftScheduler.cpp
 *
 *  Created on: Jan 26, 2019
 *      Author: erics
 */

#include "SoftScheduler.h"

namespace std {
  void __throw_bad_alloc()
  {
	  DEBUGSERIAL.println("Unable to allocate memory");
  }

  void __throw_length_error( char const*e )
  {
	  DEBUGSERIAL.print("Length Error :");
	  DEBUGSERIAL.println(e);
  }
}

double approxRollingAverage (double avg, double input) {
    avg -= avg/256.0;
    avg += input/256.0;
    return avg;
}

void SoftScheduler::init() {
	// do we actually need to init anything? maybe clear the queue?
}

void SoftScheduler::addTask(void (*fun_ptr)(), uint32_t interval)
{
	addTask(fun_ptr, interval, 0, "null");
}

void SoftScheduler::addTask(void (*fun_ptr)(), uint32_t interval, uint32_t delay, String name)
{
	Task newtask;
	newtask.fun_ptr = fun_ptr;
	newtask.interval = interval;
	newtask.lastExecuteTime = millis()-interval+delay;
	newtask.name = name;

	taskList.push_back(newtask);
	if(taskList.size() > 50)
	{
		Telemetry::printf(MSG_WARNING, "The scheduler queue is more than 50 deep!");
		DEBUGSERIAL.printf("The scheduler queue is more than 50 deep!");
	}
}

void SoftScheduler::tickOnce()
{
	if(!taskList.empty())
	{
		Task currentTask = taskList.front();
		if(currentTask.interval == 0)
		{
			taskList.pop_front();
			currentTask.fun_ptr();
			return;
		}
		else
		{
			uint32_t currentTime = millis();
			uint16_t diffTime = currentTime - currentTask.lastExecuteTime;
			int16_t jitter = diffTime-currentTask.interval;
			if(jitter >= 0)
			{
				if(jitter >= 10) Telemetry::printf(MSG_WARNING, "jitter for this scheduled task [%s] was %ld ms!\n", currentTask.name.c_str(), jitter);
				if(averageJitter >= 1) Telemetry::printf(MSG_WARNING, "average schedule jitter is %0.2f ms!\n", averageJitter);
				currentTask.lastExecuteTime = currentTime;
				currentTask.fun_ptr();
				averageJitter = approxRollingAverage(averageJitter, diffTime-currentTask.interval);
			}
			taskList.pop_front();
			taskList.push_back(currentTask);
		}
	}
}

double SoftScheduler::getAverageJitter()
{
	return averageJitter;
}

int SoftScheduler::getQueueSize()
{
	return taskList.size();
}
