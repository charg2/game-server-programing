#pragma once
//#include "../C2Server/C2Server/concurrency/MPSCQueue.h"
#include "../C2Server/C2Server/enviroment.h"
#include "MMONpc.h"

struct MMOZone;
class MMOServer;
struct MMONpcManager
{
	MMONpcManager();

	void initilize();
	void place_npc_in_zone();

	MMONpc* get_npc(uint64_t sever_id);

public:
	void set_zone(MMOZone* zone);

private:
	MMONpc*		npcs;
	MMOServer*	server;
	MMOZone*	zone;
};

extern MMONpcManager* g_npc_manager;