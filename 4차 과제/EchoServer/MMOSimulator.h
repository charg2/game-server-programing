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
	//MMOZone[] zones;  // �������� ������ �о�ͼ� ���� �� �ֶǷ� �� �� ������ �����.
};

