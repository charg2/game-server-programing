#include "MMONPCManager.h"
#include "MMOZone.h"

#include <string>

MMONpcManager* g_npc_manager{};

MMONpcManager::MMONpcManager()
{}

void MMONpcManager::initilize()
{
	npcs = new MMONPC[MMONpcManager::max_npc_count]{};

	int npc_type_dividers[NT_MAX]{};

	int dived_counter = 0;
	for (int& a : npc_type_dividers)
	{
		a = ( MMONpcManager::max_npc_count / NT_MAX ) * NPCType(dived_counter + 1);
		dived_counter += 1;
		printf(" a : %d\n", a);
	}

	

	NPCType prev_npc_type{ NT_PEACE_FIXED };
	for (size_t idx{}; idx < MMONpcManager::max_npc_count; ++idx)
	{
		NPCType npc_type;
		for (npc_type = prev_npc_type; npc_type < NPCType( NT_MAX - 1 ); npc_type = NPCType( npc_type + 1 ))
		{
			if ( idx < npc_type_dividers[npc_type] )
				break;
		}

		npcs[idx].type = npc_type;
		npcs[idx].initialize(idx);
		npcs[idx].initialize_vm_and_load_data();

		prev_npc_type = npc_type;
	}
}

void MMONpcManager::place_npc_in_zone()
{
	MMOZone* zone = this->zone; 

	for (size_t n{}; n < MMONpcManager::max_npc_count; ++n)
	{
		MMOSector* sector = zone->get_sector(npcs[n].y, npcs[n].x);
		
		sector->npcs.insert(npcs[n].id);

		npcs[n].current_sector = sector;
	}
}

MMONPC* MMONpcManager::get_npc(uint64_t server_id)
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
