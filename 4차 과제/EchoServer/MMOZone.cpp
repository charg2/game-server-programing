#include "../C2Server/C2Server/pre_compile.h"
#include "MMOActor.h"
#include "MMOZone.h"
#include "MMONear.h"
#include "MMOServer.h"

MMOZone::MMOZone()
{
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
	printf("MMOSimulator Ready... Ok\n ");
	//
}

MMOZone::~MMOZone()
{
}


void MMOZone::accept_actor(MMOActor* actor)
{
	this->actors.insert(std::make_pair(actor->get_id(), actor));
	actor->set_zone(this);

	MMOSector* dest_sector = &sectors[actor->get_y()][actor->get_x()];
	dest_sector->accept_actor(actor->get_session_id(), actor);
}

void MMOZone::release_actor(MMOActor* actor)
{
	this->actors.erase(actor->get_id());

	//
}

void MMOZone::dispatch_network()
{

}

void MMOZone::simulate_interaction()
{
	for (int y = 0; y < c2::constant::MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < c2::constant::MAP_WIDTH; ++x)
		{
			sectors[y][x].simulate();
		}
	}

//	가비지 컬렉션.
	for (uint16_t id : garbages)
	{
		MMOSector* cur_sector = actors[id]->get_current_sector();
		cur_sector->leave_actor(id);
	}

	garbages.clear();
}



void MMOZone::broadcaset_all()
{
}

void MMOZone::broadcaset_nearby_others()
{
}

MMOSector* MMOZone::get_sector(int32_t y, int32_t x)
{
	return &this->sectors[postion_width_mapling_table[y]][postion_width_mapling_table[x]];
}

void MMOZone::add_garbage(int16_t id)
{
	garbages.push_back(id);
}

void MMOZone::erase_session(uint64_t session_id)
{
	MMOActor* actor = actors[(int16_t)session_id];
	size_t ret = actors.erase((int16_t)session_id);
	if (nullptr == actor )
	{
		return;
	}
	if (ret == 0)
	{
		return;
	}

	MMOServer* server = MMOSimulator::get_instance().get_server();

	sc_packet_leave leave_payload;
	leave_payload.header.length = sizeof(sc_packet_leave);
	leave_payload.header.type = S2C_LEAVE;
	leave_payload.id = actor->get_id();

	MMOSector* current_sector = actor->current_sector;
	MMONear* near_sectors = current_sector->get_near(actor->y, actor->x);
	int near_cnt = near_sectors->count;
	for (int c = 0; c < near_cnt; ++c)
	{
		auto actors = near_sectors->sectors[c]->actors;
		for (auto& it : actors)
		{
			MMOActor* other = it.second;

			c2::Packet* leave_packet = c2::Packet::alloc();
			leave_packet->write(&leave_payload, sizeof(sc_packet_leave));
			server->send_packet(other->get_session_id(), leave_packet);
		}
	}




}

