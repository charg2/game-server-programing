#pragma once
#include "MMOSector.h"
#include "../C2Server/C2Server/concurrency/MPSCQueue.h"

using namespace c2::concurrency;
class MMOZone
{
public:
	MMOZone();
	MMOZone(const MMOZone& other) = delete;
	MMOZone(MMOZone&& other) = delete;
	~MMOZone();

	void dispatch_network();
	void simulate_interaction();

	void broadcaset_all();
	void broadcaset_nearby_others();

private:
	void process_packet();

private:
	MMOSector		 sectors[800][800];
	MPSCQueue<void*> message_queue;
	
};

