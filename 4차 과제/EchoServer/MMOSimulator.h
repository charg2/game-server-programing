#pragma once
#include "MMOMessage.h"
#include "../C2Server/C2Server/concurrency/MPSCQueue.h"

using namespace c2::concurrency;
class MMOZone;
class MMOServer;
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
	void put_disconnected_session(uint64_t session_id);
	void set_server(MMOServer* server);
	MMOServer* get_server();

private:
	MMOZone*				zones;// []
	MPSCQueue<uint64_t>*	disconnected_session_id;
	MPSCQueue<MMOMessage>*	message_queue;
	MMOServer*				server;
};

