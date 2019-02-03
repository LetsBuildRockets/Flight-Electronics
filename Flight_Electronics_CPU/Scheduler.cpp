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

		highPriorty_fast_intervals[currentTask.priority][highPriorty_fast_taskListSize[currentTask.priority]] = currentTask.interval;
		highPriorty_fast_ISRs[currentTask.priority][highPriorty_fast_taskListSize[currentTask.priority]] = currentTask.fun_ptr;
		DEBUGSERIAL.printf("addr of func at start: %x\t", highPriorty_fast_ISRs[currentTask.priority][highPriorty_fast_taskListSize[currentTask.priority]]);
		highPriorty_fast_lastExec[currentTask.priority][highPriorty_fast_taskListSize[currentTask.priority]] = 0;
		highPriorty_fast_taskListSize[currentTask.priority]++;

		DEBUGSERIAL.printf("Name: %s, priortity %d\n", currentTask.name.c_str(), currentTask.priority);

	}

	for (int i  = 0; i < 3; i++)
	{
		for(int j = 0; j < highPriorty_fast_taskListSize[i]; j++)
		{
			DEBUGSERIAL.printf("priortity %d, funcp: %x\n", i, highPriorty_fast_ISRs[i][j]);
		}
	};

	if(!hwTimerRunning[HIGH_PRIORITY_253] && highPriorty_fast_taskListSize[HIGH_PRIORITY_253] > 0)
	{
		hwTimer[HIGH_PRIORITY_253].priority(253);
		hwTimer[HIGH_PRIORITY_253].begin(tickHard_253, 100);
		hwTimerRunning[HIGH_PRIORITY_253] = true;
	}
	if(!hwTimerRunning[HIGH_PRIORITY_254] && highPriorty_fast_taskListSize[HIGH_PRIORITY_254] > 0)
	{
		hwTimer[HIGH_PRIORITY_254].priority(254);
		hwTimer[HIGH_PRIORITY_254].begin(tickHard_254, 100);
		hwTimerRunning[HIGH_PRIORITY_254] = true;
	}

	if(!hwTimerRunning[HIGH_PRIORITY_255] && highPriorty_fast_taskListSize[HIGH_PRIORITY_255] > 0)
	{
		hwTimer[HIGH_PRIORITY_255].priority(255);
		hwTimer[HIGH_PRIORITY_255].begin(tickHard_255, 100);
		hwTimerRunning[HIGH_PRIORITY_255] = true;
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
	newtask.lastExecuteTime = micros()+delay;
	newtask.name = name;
	newtask.priority = taskPriortiy;

	if(taskPriortiy == LOW_PRIORITY)
	{
		taskList_LOW_PRIORITY.push(newtask);
		if(taskList_LOW_PRIORITY.size() > 50)
		{
			Telemetry::printf(MSG_WARNING, "The lOW_PRIORITY scheduler queue is more than 50 deep!");
		}
	}
	else if(taskPriortiy == HIGH_PRIORITY_255 || taskPriortiy == HIGH_PRIORITY_254 || taskPriortiy == HIGH_PRIORITY_253)
	{
		if(!hwTimerRunning[taskPriortiy])
		{
			if(taskList_HIGH_PRIORITY.size() > 50)
			{
				Telemetry::printf(MSG_ERROR, "The HIGH_PRIORITY scheduler queue is more than 50 deep!");
			}
			else
			{
				taskList_HIGH_PRIORITY.push(newtask);
				//DEBUGSERIAL.printf("addr of func: %x\n", newtask.fun_ptr);
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
		taskList_LOW_PRIORITY.pop();

		uint32_t currentTime = micros();
		if(currentTime > currentTask.lastExecuteTime) {
			uint32_t elapsed = currentTime - currentTask.lastExecuteTime;
			if(elapsed > currentTask.interval)
			{
				DEBUGSERIAL.printf("Name: %s, elapsed: %lu\n", currentTask.name.c_str(), elapsed);
				uint16_t jitter = elapsed-currentTask.interval;
				if(jitter >= 10000) Telemetry::printf(MSG_WARNING, "jitter for this scheduled task [%s] was %ld us!\n", currentTask.name.c_str(), jitter);
				if(averageJitter >= 1000) Telemetry::printf(MSG_WARNING, "average schedule jitter is %0.2f us!\n", averageJitter);
				currentTask.lastExecuteTime = currentTime;
				currentTask.fun_ptr();
				averageJitter = approxRollingAverage(averageJitter, jitter);
				if(currentTask.interval == 0) return;
			}
		}
		taskList_LOW_PRIORITY.push(currentTask);
	}
}

void Scheduler::tickHard_255() {
	tickHard(HIGH_PRIORITY_255);
}
void Scheduler::tickHard_254() {
	tickHard(HIGH_PRIORITY_254);
}
void Scheduler::tickHard_253() {
	tickHard(HIGH_PRIORITY_253);
}

void Scheduler::tickHard(uint8_t listIndex)
{
	uint32_t currentTime = micros();

	__disable_irq();

	ISR fun_ptr = highPriorty_fast_ISRs[listIndex][highPriorty_fast_index[listIndex]];
	uint32_t lastExecuteTime = highPriorty_fast_lastExec[listIndex][highPriorty_fast_index[listIndex]]; // in microseconds
	if(currentTime > lastExecuteTime) {
		uint32_t interval = highPriorty_fast_intervals[listIndex][highPriorty_fast_index[listIndex]]; // in microseconds
		uint32_t elapsed = currentTime - lastExecuteTime;
		if(elapsed > interval)
		{
			//uint16_t jitter = elapsed-interval;
			//if(jitter >= 10000) Telemetry::printf(MSG_WARNING, "jitter for this scheduled task [%s] was %ld ms!\n", currentTask.name.c_str(), jitter);
			//if(averageJitter >= 1000) Telemetry::printf(MSG_WARNING, "average schedule jitter is %0.2f us!\n", averageJitter);
			highPriorty_fast_lastExec[listIndex][highPriorty_fast_index[listIndex]] = currentTime;
			DEBUGSERIAL.printf("running: %x at priortiylevel: %d\n", fun_ptr, listIndex);
			DEBUGSERIAL.flush();
			__enable_irq();
			//averageJitter = approxRollingAverage(averageJitter, jitter);

			fun_ptr();
			//DEBUGSERIAL.printf("addr of func in isr: %x, index is: %d\n", fun_ptr,highPriorty_fast_index);
			__disable_irq();
		}
	}
	highPriorty_fast_index[listIndex]++;
	if(highPriorty_fast_index[listIndex] >= highPriorty_fast_taskListSize[listIndex]) highPriorty_fast_index[listIndex] = 0;
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
