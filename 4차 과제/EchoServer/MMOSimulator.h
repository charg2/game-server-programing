#pragma once

//#include "../1Common/concurrency/ConcurrentQueue.h"

#include "MMOPlayer.h"

class MMOPlayer;

class MMOSimulator
{
public:
	MMOSimulator();
	~MMOSimulator();

	void dispatch_netwrok();
	void simulate_interaction();

	// lock free queue
	//c2::concurrency::ConcurrentQueue<> message_queue;
	// MPSCQueue
	MMOPlayer** plaers;
	//MMOZone[] zones;  // 동적으로 지형을 읽어와서 만들 수 있또록 할 수 있을지 고민임.
};

