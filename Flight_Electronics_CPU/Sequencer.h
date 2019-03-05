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

#define COUNTDOWN_START -30 // start at T-30 Seconds

struct SeqTask {
	int32_t startTime; // in millis
	uint16_t duration; // in millis
	void (*functionPtr)(void);
	const char * comment;
	volatile struct SeqTask * nextSeqTask;
};

enum SeqState {ABORT, HOLD, RUNNING};

class Sequencer
{
	public:
		Sequencer();
		~Sequencer();
		void addSeqTask(struct SeqTask newSeqTask);
		void tick();
		void printTaskList();

	private:
		static volatile SeqState state;
		static volatile struct SeqTask * firstSeqTask;
		static volatile struct SeqTask * listHead;
		static volatile uint16_t startTime;
};

#endif /* SEQUENCER_H_ */
