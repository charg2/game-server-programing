#include "../C2Server/C2Server/pre_compile.h"
#include "MMOActor.h"
#include "MMOZone.h"
#include "MMOServer.h"

MMOZone::MMOZone()
{
	InitializeSRWLock(&lock);

	// position to idx table
	int idx_y = 0, idx_x = 0;
	for (int y = 0; y < c2::constant::MAP_HEIGHT; y += 1)
	{
		if (y % c2::constant::SECTOR_HEIGHT == 0 && y != 0)
			idx_y += 1;

		postion_height_mapling_table[y] = idx_y;
	}
	
	for (int x = 0; x < c2::constant::MAP_WIDTH; x += 1)
	{
		if (x % c2::constant::SECTOR_WIDTH == 0 && x != 0)
			idx_x += 1;

		postion_width_mapling_table[x] = idx_x;
	}
	

////////////
	for (int y = 0; y < c2::constant::MAP_NAX_HEIGHT_INDEX; y += 1)
	{
		for (int x = 0; x < c2::constant::MAP_NAX_WIDTH_INDEX; x += 1)
		{
			sectors[y][x].index_y = y;
			sectors[y][x].index_x = x;
			sectors[y][x].zone = this;
			sectors[y][x].calculate_serctor_idx(); // 존을 늘린다면 밖으로 뺴자;
		}
	}

	printf("init map... Ok\n ");
	printf("MMOZone Ready... Ok\n ");
}

MMOZone::~MMOZone()
{
}


void MMOZone::accept_actor(MMOActor* actor)
{
	//this->actors.insert(std::make_pair(actor->get_id(), actor));
	//actor->zone = this;

	//MMOSector* dest_sector = &sectors[actor->y][actor->x];
	//dest_sector->accept_actor(actor->session_id, actor);
}

void MMOZone::release_actor(MMOActor* actor)
{
	//this->actors.erase(actor->get_id());
}


void MMOZone::broadcaset()
{
	// 순회하면서 발사.


}

MMOSector* MMOZone::get_sector(MMOActor* actor)
{
	return &this->sectors[postion_width_mapling_table[actor->y]][postion_width_mapling_table[actor->x]];
}

MMOSector* MMOZone::get_sector(int32_t y, int32_t x)
{
	return &this->sectors[postion_width_mapling_table[y]][postion_width_mapling_table[x]];
}

MMOSector* MMOZone::calc_near_sector(int32_t y, int32_t x)
{
	using namespace c2::constant;
	int l_min_y = max(y - FOV_HALF_HEIGHT, 0), l_min_x = max(x - FOV_HALF_WIDTH,  0);
	int l_max_y = min(y + FOV_HALF_HEIGHT, FOV_HEIGHT -1), l_max_x = min(x + FOV_HALF_WIDTH, FOV_HALF_WIDTH -1);

	MMOSector* current_sector1 = get_sector(l_max_y, l_max_x);
	MMOSector* current_sector2 = get_sector(l_max_y, l_min_x);
	MMOSector* current_sector3 = get_sector(l_min_y, l_max_x);
	MMOSector* current_sector4 = get_sector(l_min_y, l_min_x);
}

void MMOZone::erase_session(uint64_t session_id)
{
	/*MMOActor* actor = actors[(int16_t)session_id];
	size_t ret = actors.erase((int16_t)session_id);
	if (nullptr == actor )
	{
		return;
	}
	if (ret == 0)
	{
		return;
	}


	sc_packet_leave leave_payload;
	leave_payload.header.length = sizeof(sc_packet_leave);
	leave_payload.header.type = S2C_LEAVE;
	leave_payload.id = actor->get_id();


	MMOSector* current_sector = actor->current_sector;*/
	//std::vector<MMOSector*>& near_sectors = current_sector->near_sectors;
	//for (MMOSector* sector : near_sectors)
	//{
	//	auto actors = sector->actors;
	//	for( auto& it : actors )
	//	{
	//		MMOActor* other = it.second;
	//		
	//		c2::Packet* leave_packet =  c2::Packet::alloc();
	//		leave_packet->write(&leave_payload, sizeof(sc_packet_leave));
	//		server->send_packet(other->session_id, leave_packet);
	//	}
	//}
}

