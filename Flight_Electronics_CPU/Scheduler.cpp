/*
 * Scheduler.cpp
 *
 *  Created on: Jan 26, 2019
 *      Author: erics
 */

#include "Scheduler.h"
namespace std {
  void __throw_bad_alloc()
  {
	  DEBUGSERIAL.println("Unable to allocate memory");
	  while(1);
  }

  void __throw_length_error( char const*e )
  {
	  DEBUGSERIAL.print("Length Error :");
	  DEBUGSERIAL.println(e);
	  while(1);
  }
}

double approxRollingAverage (double avg, double input) {
    avg -= avg/256.0;
    avg += input/256.0;
    return avg;
}

void Scheduler::init() {
	// do we actually need to init anything? maybe clear the queue?
}

void Scheduler::startHwTimer()
{

	while(taskList_HIGH_PRIORITY.size() > 0)
	{
		Task currentTask = taskList_HIGH_PRIORITY.front();
		taskList_HIGH_PRIORITY.pop();

		highPriortyInvertvals_fast[highPriortyTaskListSize] = currentTask.interval;
		highPriortyTasks_fast[highPriortyTaskListSize] = currentTask.fun_ptr;
		DEBUGSERIAL.printf("addr of func at start: %x\n", highPriortyTasks_fast[highPriortyTaskListSize]);
		highPriortyLastExec_fast[highPriortyTaskListSize] = 0;
		highPriortyTaskListSize++;
	}

	if(!hwTimerRunning && highPriortyTaskListSize > 0)
	{
		hwTimer.priority(250);
		hwTimer.begin(tickHard, 100);
		hwTimerRunning = true;
	}
}


void Scheduler::addTask(TASK_PRIORITY taskPriortiy, ISR fun_ptr, uint32_t interval)
{
	addTask(taskPriortiy, fun_ptr, interval, 0, "null");
}

void Scheduler::addTask(TASK_PRIORITY taskPriortiy, ISR fun_ptr, uint32_t interval, uint32_t delay, String name)
{
	Task newtask;
	newtask.fun_ptr = fun_ptr;
	newtask.interval = interval;
	newtask.lastExecuteTime = micros()-interval+delay;
	newtask.name = name;

	if(taskPriortiy == LOW_PRIORITY)
	{
		taskList_LOW_PRIORITY.push(newtask);
		if(taskList_LOW_PRIORITY.size() > 50)
		{
			Telemetry::printf(MSG_WARNING, "The lOW_PRIORITY scheduler queue is more than 50 deep!");
		}
	}
	else if(taskPriortiy == HIGH_PRIORITY)
	{
		if(!hwTimerRunning)
		{
			if(taskList_LOW_PRIORITY.size() > 50)
			{
				Telemetry::printf(MSG_ERROR, "The HIGH_PRIORITY scheduler queue is more than 50 deep!");
			}
			else
			{
				taskList_HIGH_PRIORITY.push(newtask);
				DEBUGSERIAL.printf("addr of func: %x\n", newtask.fun_ptr);
			}
		}
		else
		{
			Telemetry::printf(MSG_ERROR, "We can't add to the high priority list while the HW Timer is running!");
		}
	}
}

void Scheduler::tickSoft()
{
	if(!taskList_LOW_PRIORITY.empty())
	{
		Task currentTask = taskList_LOW_PRIORITY.front();
		if(currentTask.interval == 0)
		{
			taskList_LOW_PRIORITY.pop();
			currentTask.fun_ptr();
			return;
		}
		else
		{
			uint32_t currentTime = micros();
			uint32_t elapsed = currentTime - currentTask.lastExecuteTime;
			if(elapsed > currentTask.interval)
			{
				uint16_t jitter = elapsed-currentTask.interval;
				if(jitter >= 10000) Telemetry::printf(MSG_WARNING, "jitter for this scheduled task [%s] was %ld ms!\n", currentTask.name.c_str(), jitter);
				if(averageJitter >= 1000) Telemetry::printf(MSG_WARNING, "average schedule jitter is %0.2f us!\n", averageJitter);
				currentTask.lastExecuteTime = currentTime;
				currentTask.fun_ptr();
				averageJitter = approxRollingAverage(averageJitter, jitter);
			}
			taskList_LOW_PRIORITY.pop();
			taskList_LOW_PRIORITY.push(currentTask);
		}
	}
}

void Scheduler::tickHard()
{
	uint32_t currentTime = micros();

	__disable_irq();

	ISR fun_ptr = highPriortyTasks_fast[highPriortytaskIndex_fast];
	uint32_t lastExecuteTime = highPriortyLastExec_fast[highPriortytaskIndex_fast]; // in microseconds
	uint32_t interval = highPriortyInvertvals_fast[highPriortytaskIndex_fast]; // in microseconds
	uint32_t elapsed = currentTime - lastExecuteTime;
	if(elapsed > interval)
	{
		//uint16_t jitter = elapsed-interval;
		//if(jitter >= 10000) Telemetry::printf(MSG_WARNING, "jitter for this scheduled task [%s] was %ld ms!\n", currentTask.name.c_str(), jitter);
		//if(averageJitter >= 1000) Telemetry::printf(MSG_WARNING, "average schedule jitter is %0.2f us!\n", averageJitter);
		highPriortyLastExec_fast[highPriortytaskIndex_fast] = currentTime;

		__enable_irq();
		//averageJitter = approxRollingAverage(averageJitter, jitter);

		fun_ptr();
		//DEBUGSERIAL.printf("addr of func in isr: %x, index is: %d\n", fun_ptr,highPriortytaskIndex_fast);
		__disable_irq();
	}

	highPriortytaskIndex_fast++;
	if(highPriortytaskIndex_fast >= highPriortyTaskListSize) highPriortytaskIndex_fast = 0;
	__enable_irq();
}

double Scheduler::getAverageJitter()
{
	return averageJitter;
}

int Scheduler::getQueueSize()
{
	return taskList_LOW_PRIORITY.size();
}
