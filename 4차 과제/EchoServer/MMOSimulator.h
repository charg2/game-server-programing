#pragma once
#include "MMOZone.h"
#include "../C2Server/C2Server/concurrency/MPSCQueue.h"

using namespace c2::concurrency;

class MMOMessage;
class MMOSimulator
{
	MMOSimulator();
	MMOSimulator(const MMOSimulator& other) = delete; 
	MMOSimulator(MMOSimulator&& other) noexcept = delete;

public:
	static MMOSimulator& get_instance();
	
	void dispatch();
	void simulate();
	void put_message(MMOMessage* message);

private:
	MMOZone					zones;// []
	MPSCQueue<MMOMessage*>	message_queue;
};

