#include "MMONpcManager.h"
#include "MMOZone.h"

#include <string>

void MMONpcManager::initilize()
{
	for (size_t n{}; n < c2::constant::MAX_NPC; ++n)
	{
		npcs[n].current_sector = nullptr;
		npcs[n].hp = 200;
		npcs[n].id = n + c2::constant::NPC_ID_OFFSET;
		npcs[n].max_hp = 200;
		std::string str = "NPC_" + n;
		strcpy_s(npcs[n].name, str.size() ,str.c_str());
		npcs[n].target = nullptr;
		npcs[n].x = rand() % c2::constant::MAP_WIDTH;
		npcs[n].y = rand() % c2::constant::MAP_HEIGHT;
		npcs[n].zone = this->zone;
	}
}

void MMONpcManager::place_npc_in_zone()
{
	MMOZone* zone = this->zone;
	
	for (size_t n{}; n < c2::constant::MAX_NPC; ++n)
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


MMONpcManager& MMONpcManager::instance()
{
	static MMONpcManager inst{};
	return inst;
}

void MMONpcManager::set_zone(MMOZone* zone)
{
	this->zone = zone;
}
