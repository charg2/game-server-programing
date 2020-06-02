#pragma once
//#include "../C2Server/C2Server/concurrency/MPSCQueue.h"
#include "../C2Server/C2Server/enviroment.h"
#include "MMONpc.h"

class MMOZone;
class MMOServer;
struct MMONpcManager
{
	MMONpcManager();

	void initilize();
	void place_npc_in_zone();

	MMONpc* get_npc(uint64_t sever_id);

public:
	static MMONpcManager& instance();
	void set_zone(MMOZone* zone);

private:
	MMONpc		npcs[c2::constant::MAX_NPC];
	MMOServer*	server;
	MMOZone*	zone;
};

