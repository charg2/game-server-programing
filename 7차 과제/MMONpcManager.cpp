#include "MMONpcManager.h"
#include "MMOZone.h"

#include <string>

MMONpcManager* g_npc_manager{};

MMONpcManager::MMONpcManager()
{}

void MMONpcManager::initilize()
{
	npcs = new MMONpc[MMONpcManager::max_npc_count]{};

	for (size_t n{}; n < MMONpcManager::max_npc_count; ++n)
	{
		npcs[n].initialize(n);
	}
}

void MMONpcManager::place_npc_in_zone()
{
	MMOZone* zone = this->zone;
	
	for (size_t n{}; n < MMONpcManager::max_npc_count; ++n)
		//for (size_t n{}; n < c2::constant::MAX_NPC; ++n)
	{
		MMOSector* sector =  zone->get_sector(npcs[n].y, npcs[n].x);
		
		sector->npcs.insert(npcs[n].id);

		npcs[n].current_sector = sector;
	}
}

MMONpc* MMONpcManager::get_npc(uint64_t server_id)
{
	return &npcs[server_id- c2::constant::NPC_ID_OFFSET];
}


void MMONpcManager::set_zone(MMOZone* zone)
{
	this->zone = zone;
}

void MMONpcManager::set_max_npc_count(uint32_t max_npc_count)
{
	MMONpcManager::max_npc_count = max_npc_count;
}
