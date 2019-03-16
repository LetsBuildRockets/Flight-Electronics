/*
 * Sequencer.cpp
 *
 *  Created on: Mar 5, 2019
 *      Author: erics
 */

#include "Sequencer.h"

Sequencer::Sequencer(int32_t countDownStartTime)
{
	firstSeqTask = (SeqTask*)malloc(sizeof(SeqTask));
	listHead = firstSeqTask;

	if(firstSeqTask == NULL)
	{
		Telemetry::printf(MSG_ERROR, "unable to allocate memory for new task sequence!\n");
		return;
	}

	firstSeqTask->comment = "Initialization";
	firstSeqTask->startTime = countDownStartTime;
	firstSeqTask->duration = 0;
	firstSeqTask->functionPtr = ([]() {Telemetry::printf(MSG_INFO, "Sequencer Init");});
	firstSeqTask->nextSeqTask = NULL;

	seqStartTime = 0;
	state = HAVENOTRUNYET;
}

Sequencer::~Sequencer()
{
	struct SeqTask *head = (struct SeqTask*) firstSeqTask;
	while(head != NULL)
	{
		struct SeqTask *next = (struct SeqTask*)head->nextSeqTask;
		free(head);
		head = next;
	}
}

int Sequencer::addSeqTask(struct SeqTask newSeqTask)
{
	return addSeqTask(newSeqTask.startTime, newSeqTask.duration, newSeqTask.functionPtr, newSeqTask.comment);
}


int Sequencer::addSeqTask(int32_t startTime, uint16_t duration, void (*functionPtr)(void), const char * comment)
{
	struct SeqTask *head = (struct SeqTask*) firstSeqTask;
	Serial.printf("next: %p\r\n",(head->nextSeqTask));
	if(head == NULL) return SEQ_ERR_NO_HEAD;
	while(head->nextSeqTask != NULL){
		head = (struct SeqTask*) head->nextSeqTask;
		Serial.printf("\tnext: %p\r\n",(head->nextSeqTask));
	}
	if(head->startTime >= startTime)
	{
		Telemetry::printf(MSG_ERROR, "you tried to add a task to a sequence that is out of order!");
		return SEQ_ERR_OUTOFORDER;
	}
	volatile struct SeqTask *anotherSeqTask = (struct SeqTask*)malloc(sizeof(struct SeqTask));
	if(anotherSeqTask == NULL)
	{
		Telemetry::printf(MSG_ERROR, "unable to allocate memory to add another task!\n");
		return SEQ_ERR_NO_MEM;
	}
	anotherSeqTask->comment = comment;
	anotherSeqTask->startTime = startTime;
	anotherSeqTask->duration = duration;
	anotherSeqTask->functionPtr = functionPtr;
	anotherSeqTask->nextSeqTask = NULL;
	head->nextSeqTask=anotherSeqTask;
	return SEQ_ERR_NONE;
}

void Sequencer::printTaskList()
{
	struct SeqTask *head = (struct SeqTask*) firstSeqTask;
	Serial.println("Sequencer Task List: ");
	while(head != NULL)
	{
		Telemetry::printf(MSG_INFO, "%8dms: %6lums, %s\n", head->startTime, head->duration, head->comment);
		head = (struct SeqTask*) head->nextSeqTask;
	}
}

void Sequencer::setState(SeqState newState)
{
	state = newState;
}

void Sequencer::start()
{
	if(state == HAVENOTRUNYET)
	{
		state = RUNNING;
		seqStartTime = millis();
	}
	else
	{
		Telemetry::printf(MSG_WARNING, "I tried to start the sequencer, but its already running!");
	}
}

void Sequencer::reset()
{
	state = HAVENOTRUNYET;
	seqStartTime = 0;
	listHead = firstSeqTask;
}

void Sequencer::tick()
{
	int32_t countdownTime = millis() - seqStartTime + firstSeqTask->startTime;
	switch(state)
	{
		case(DONE):
			break;
		case(HAVENOTRUNYET):
			break;
		case(HOLD):
			break;
		case(RUNNING):
		{
			struct SeqTask *task = (struct SeqTask*) listHead;
			if(countdownTime - task->startTime > 0)
			{
				if(task->functionPtr != NULL) task->functionPtr();
				if(countdownTime - task->startTime >= task->duration)
				{
					task = (struct SeqTask*) task->nextSeqTask;
					if(task == NULL)
					{
						Telemetry::printf(MSG_INFO, "Sequence done");
						state = DONE;
					}
					else
					{
						listHead = task;
					}
				}
			}

			break;
		}
		default:
			Telemetry::printf(MSG_ERROR, "INVALID sequencer state: %u", (unsigned int)state);
	}
}
