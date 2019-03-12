/*
 * Sequencer.h
 *
 *  Created on: Mar 5, 2019
 *      Author: erics
 */

#ifndef SEQUENCER_H_
#define SEQUENCER_H_

#include <Arduino.h>

#include "Telemetry.h"

#define SEQ_ERR_NONE	      0
#define SEQ_ERR_NO_MEM		 -1
#define SEQ_ERR_NO_HEAD		 -2
#define SEQ_ERR_OUTOFORDER	 -3

struct SeqTask {
	int32_t startTime; // in millis
	uint16_t duration; // in millis
	void (*functionPtr)(void);
	const char * comment;
	volatile struct SeqTask * nextSeqTask;
};

enum SeqState {HAVENOTRUNYET, HOLD, RUNNING, DONE};

class Sequencer
{
	public:
		Sequencer(int32_t coundownStartTime);
		~Sequencer();
		int addSeqTask(struct SeqTask newSeqTask);
		int addSeqTask(int32_t startTime, uint16_t duration, void (*functionPtr)(void), const char * comment);
		void tick();
		void printTaskList();
		void setState(SeqState newState);
		void reset();
		void start();

	private:
		volatile SeqState state;
		volatile struct SeqTask * firstSeqTask;
		volatile struct SeqTask * listHead;
		volatile int32_t seqStartTime;
};

#endif /* SEQUENCER_H_ */
