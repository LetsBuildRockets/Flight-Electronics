/*
 * Sequencer.cpp
 *
 *  Created on: Mar 5, 2019
 *      Author: erics
 */

#include "Sequencer.h"

volatile uint16_t Sequencer::startTime = millis();
volatile struct SeqTask* Sequencer::firstSeqTask = (SeqTask*)malloc(sizeof(SeqTask));
volatile struct SeqTask* Sequencer::listHead = Sequencer::firstSeqTask;
volatile SeqState Sequencer::state = HOLD;

Sequencer::Sequencer()
{
	firstSeqTask->comment = "Initialization";
	firstSeqTask->startTime = COUNTDOWN_START*1000l;
	firstSeqTask->duration = 0;
	firstSeqTask->functionPtr = ([]() {Telemetry::printf(MSG_INFO, "Sequencer Init");});

	startTime = millis();
}

Sequencer::~Sequencer()
{
	struct SeqTask *head = (struct SeqTask*)firstSeqTask;
	while(head != NULL)
	{
		struct SeqTask *next = (struct SeqTask*)head->nextSeqTask;
		free((void*)head);
		head = next;
	}
}

void Sequencer::addSeqTask(struct SeqTask newSeqTask)
{
	struct SeqTask *head = (struct SeqTask*) firstSeqTask;
	if(head == NULL) Sequencer(); // i guess i can do this...
	while(head->nextSeqTask != NULL) head = (struct SeqTask*) head->nextSeqTask;
	volatile struct SeqTask *anotherSeqTask = (struct SeqTask*)malloc(sizeof(struct SeqTask));
	anotherSeqTask->comment = newSeqTask.comment;
	anotherSeqTask->startTime = newSeqTask.startTime;
	anotherSeqTask->duration = newSeqTask.duration;
	anotherSeqTask->functionPtr = newSeqTask.functionPtr;
	anotherSeqTask->nextSeqTask = NULL;
	head->nextSeqTask=anotherSeqTask;
}

void Sequencer::printTaskList()
{
	struct SeqTask *head = (struct SeqTask*) firstSeqTask;
	Serial.println("Sequencer Task List: ");
	while(head != NULL)
	{
		Serial.printf("%d: %s\n", head->startTime, head->comment);
		head = (struct SeqTask*) head->nextSeqTask;
	}
}

void Sequencer::tick()
{
	switch(state)
	{
	case(ABORT):
			break;
	case(HOLD):
			break;
	case(RUNNING):
			break;
	default:
		Telemetry::printf(MSG_ERROR, "INVALID sequencer state: %u", (unsigned int)state);
	}
}
