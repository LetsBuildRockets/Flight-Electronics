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
	state = HOLD;

	if(firstSeqTask == NULL)
	{
		Telemetry::printf(MSG_ERROR, "unable to allocate memory for new task sequence!\n");
		return;
	}

	firstSeqTask->comment = "Initialization";
	firstSeqTask->startTime = countDownStartTime;
	firstSeqTask->duration = 0;
	firstSeqTask->functionPtr = ([]() {Telemetry::printf(MSG_INFO, "Sequencer Init");});

	seqStartTime = 0;
}

Sequencer::~Sequencer()
{
	struct SeqTask **head = (struct SeqTask**) firstSeqTask;
	while(head != NULL)
	{
		struct SeqTask *next = (struct SeqTask*)(*head)->nextSeqTask;
		free((void*)head);
		(*head) = next;
	}
}

int Sequencer::addSeqTask(struct SeqTask newSeqTask)
{
	return addSeqTask(newSeqTask.startTime, newSeqTask.duration, newSeqTask.functionPtr, newSeqTask.comment);
}


int Sequencer::addSeqTask(int32_t startTime, uint16_t duration, void (*functionPtr)(void), const char * comment)
{
	struct SeqTask **head = (struct SeqTask**) firstSeqTask;
	if(head == NULL) return SEQ_ERR_NO_HEAD;
	while((*head)->nextSeqTask != NULL) (*head) = (struct SeqTask*) (*head)->nextSeqTask;
	if((*head)->startTime >= startTime)
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
	(*head)->nextSeqTask=anotherSeqTask;
	return SEQ_ERR_NONE;
}

void Sequencer::printTaskList()
{
	struct SeqTask *head = (struct SeqTask*) firstSeqTask;
	Serial.println("Sequencer Task List: ");
	while(head != NULL)
	{
		Serial.printf("%dms: %ulms, %s\n", head->startTime, head->duration, head->comment);
		head = (struct SeqTask*) head->nextSeqTask;
	}
}

void Sequencer::setState(SeqState newState)
{
	state = newState;
}

void Sequencer::start()
{
	if(state != RUNNING && state != HOLD)
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
	switch(state)
	{
		case(HAVENOTRUNYET):
			break;
		case(HOLD):
			break;
		case(RUNNING):

			break;
		default:
			Telemetry::printf(MSG_ERROR, "INVALID sequencer state: %u", (unsigned int)state);
	}
}
