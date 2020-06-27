#pragma once
#include "MMONPC.h"

struct MMOZone;
class MMOServer;
struct MMONpcManager
{
	MMONpcManager();

	void initilize();
	void place_npc_in_zone();

	MMONPC* get_npc(uint64_t sever_id);

public:
	void set_zone(MMOZone* zone);
	static void set_max_npc_count(uint32_t max_npc_count);

private:
	MMONPC*					npcs;
	MMOServer*				server;
	MMOZone*				zone;
	static inline uint32_t	max_npc_count{};
};

extern MMONpcManager* g_npc_manager;

