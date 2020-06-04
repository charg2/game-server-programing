#include "MMONpcManager.h"
#include "MMOZone.h"

#include <string>

MMONpcManager* g_npc_manager{};

MMONpcManager::MMONpcManager()
{}

void MMONpcManager::initilize()
{
	npcs = new MMONpc[c2::constant::MAX_NPC]{};

	for (size_t n{}; n < c2::constant::MAX_NPC; ++n)
	{
		npcs[n].current_sector = nullptr;
		npcs[n].hp = 200;
		npcs[n].id = n + c2::constant::NPC_ID_OFFSET;
		npcs[n].max_hp = 200;
		

		const char* tt = "NPC";
		char ttt[10];
		memcpy(npcs[n].name, tt, 4);

		npcs[n].target = nullptr;
		npcs[n].x = rand() % c2::constant::MAP_WIDTH;
		npcs[n].y = rand() % c2::constant::MAP_HEIGHT;
		npcs[n].zone = this->zone;
		npcs[n].is_active = 0;

		//npcs[n].preare_vm();
	}
}

void MMONpcManager::place_npc_in_zone()
{
	MMOZone* zone = this->zone;
	
	for (size_t n{}; n < c2::constant::MAX_NPC; ++n)
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
