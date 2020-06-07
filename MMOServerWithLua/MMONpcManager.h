#pragma once
//#include "../C2Server/C2Server/concurrency/MPSCQueue.h"
#include "enviroment.h"
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
	static void set_max_npc_count(uint32_t max_npc_count);

private:
	MMONpc*					npcs;
	MMOServer*				server;
	MMOZone*				zone;
	static inline uint32_t	max_npc_count{};
};

extern MMONpcManager* g_npc_manager;

