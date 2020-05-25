#include "../C2Server/C2Server/pre_compile.h"
#include "MMOActor.h"
#include "MMOZone.h"
#include "MMOServer.h"
#include <algorithm>

MMOZone::MMOZone()
{
	//for (int y = 0; y < c2::constant::MAP_HEIGHT; ++y)
	//{
	//	for (int x = 0; x < c2::constant::MAP_WIDTH; ++x)
	//	{
	//		sectors[y][x].set_position(y, x);
	//		sectors[y][x].set_zone(this);
	//		sectors[y][x].calculate_near_sectors(); // 존을 늘린다면 밖으로 뺴자;
	//	}
	//}

	//// difference 구하기.
	//for (int y = 0; y < c2::constant::MAP_HEIGHT; ++y)
	//{
	//	for (int x = 0; x < c2::constant::MAP_WIDTH; ++x)
	//	{
	//		std::vector<MMOSector*>& near_sectors = sectors[y][x].get_near_sectors(); 
	//		
	//		// 1 x + 1  right
	//		if ( (x + 1 )< c2::constant::MAP_WIDTH)
	//		{
	//			std::vector<MMOSector*>& right_near_sectors = sectors[y][x + 1].get_near_sectors();

	//			std::vector<MMOSector*>& old_difference_sectors		= sectors[y][x].get_old_difference_sectors(NEAR_RIGHT);
	//			std::vector<MMOSector*>& new_difference_sectors		= sectors[y][x].get_new_difference_sectors(NEAR_RIGHT);
	//			std::vector<MMOSector*>& intersection_sectors		= sectors[y][x].get_intersection_sectors(NEAR_RIGHT);


	//			// 교집합 이동 후  & 기준
	//			intersection_sectors.reserve(110);
	//			std::set_intersection(near_sectors.begin(), near_sectors.end(), right_near_sectors.begin(), right_near_sectors.end(), std::back_inserter(intersection_sectors));
	//			intersection_sectors.shrink_to_fit();
	//			std::sort(intersection_sectors.begin(), intersection_sectors.end());

	//			// 이동 후 - 기준
	//			new_difference_sectors.reserve(11);
	//			std::set_difference(right_near_sectors.begin(), right_near_sectors.end(), intersection_sectors.begin(), intersection_sectors.end(), std::back_inserter( new_difference_sectors));
	//			new_difference_sectors.shrink_to_fit();

	//			// 기준 - 이동 후
	//			old_difference_sectors.reserve(11);
	//			std::set_difference(near_sectors.begin(), near_sectors.end(), intersection_sectors.begin(), intersection_sectors.end(), std::back_inserter(old_difference_sectors));
	//			old_difference_sectors.shrink_to_fit();
	//		}

	//		// 1 x - 1  left
	//		if ( 0 <= ( x - 1))
	//		{
	//			std::vector<MMOSector*>& left_near_sectors			= sectors[y][x - 1].get_near_sectors();

	//			std::vector<MMOSector*>& old_difference_sectors		= sectors[y][x].get_old_difference_sectors(NEAR_LEFT);
	//			std::vector<MMOSector*>& new_difference_sectors		= sectors[y][x].get_new_difference_sectors(NEAR_LEFT);
	//			std::vector<MMOSector*>& intersection_sectors		= sectors[y][x].get_intersection_sectors(NEAR_LEFT);

	//			// 교집합 이동 후  & 기준
	//			intersection_sectors.reserve(110);
	//			std::set_intersection(near_sectors.begin(), near_sectors.end(), left_near_sectors.begin(), left_near_sectors.end(), std::back_inserter(intersection_sectors));
	//			intersection_sectors.shrink_to_fit();
	//			std::sort(intersection_sectors.begin(), intersection_sectors.end());

	//			// 이동 후 - 기준
	//			new_difference_sectors.reserve(11);
	//			std::set_difference(left_near_sectors.begin(), left_near_sectors.end(), intersection_sectors.begin(), intersection_sectors.end(), std::back_inserter(new_difference_sectors));
	//			new_difference_sectors.shrink_to_fit();

	//			// 기준 - 이동 후
	//			old_difference_sectors.reserve(11);
	//			std::set_difference(near_sectors.begin(), near_sectors.end(), intersection_sectors.begin(), intersection_sectors.end(), std::back_inserter(old_difference_sectors));
	//			old_difference_sectors.shrink_to_fit();


	//		}


	//		// y + 1  dwon
	//		if ((y + 1) < c2::constant::MAP_HEIGHT)
	//		{
	//			std::vector<MMOSector*>& down_near_sectors = sectors[y + 1][x].get_near_sectors();

	//			std::vector<MMOSector*>& old_difference_sectors	= sectors[y][x].get_old_difference_sectors(NEAR_DOWN);
	//			std::vector<MMOSector*>& new_difference_sectors		= sectors[y][x].get_new_difference_sectors(NEAR_DOWN);
	//			std::vector<MMOSector*>& intersection_sectors		= sectors[y][x].get_intersection_sectors(NEAR_DOWN);

	//			// 교집합 이동 후  & 기준
	//			intersection_sectors.reserve(110);
	//			std::set_intersection(near_sectors.begin(), near_sectors.end(), down_near_sectors.begin(), down_near_sectors.end(), std::back_inserter(intersection_sectors));
	//			intersection_sectors.shrink_to_fit();
	//			std::sort(intersection_sectors.begin(), intersection_sectors.end());

	//			new_difference_sectors.reserve(11);
	//			std::set_difference(down_near_sectors.begin(), down_near_sectors.end(), intersection_sectors.begin(), intersection_sectors.end(), std::back_inserter(new_difference_sectors));
	//			new_difference_sectors.shrink_to_fit();
	//			// 기준 - 이동 후
	//			old_difference_sectors.reserve(11);
	//			std::set_difference(near_sectors.begin(), near_sectors.end(), intersection_sectors.begin(), intersection_sectors.end(), std::back_inserter(old_difference_sectors));
	//			old_difference_sectors.shrink_to_fit();

	//		}

	//		// 1 x - 1  up
	//		if (0 <= (y - 1))
	//		{
	//			std::vector<MMOSector*>& up_near_sectors = sectors[y - 1][x].get_near_sectors();


	//			std::vector<MMOSector*>& old_difference_sectors		= sectors[y][x].get_old_difference_sectors(NEAR_UP);
	//			std::vector<MMOSector*>& new_difference_sectors		= sectors[y][x].get_new_difference_sectors(NEAR_UP);
	//			std::vector<MMOSector*>& intersection_sectors		= sectors[y][x].get_intersection_sectors(NEAR_UP);

	//			// 교집합 이동 후  & 기준
	//			intersection_sectors.reserve(110);
	//			std::set_intersection(near_sectors.begin(), near_sectors.end(), up_near_sectors.begin(), up_near_sectors.end(), std::back_inserter(intersection_sectors));
	//			intersection_sectors.shrink_to_fit();
	//			std::sort(intersection_sectors.begin(), intersection_sectors.end());

	//			// 이동 후 - 교집합
	//			new_difference_sectors.reserve(11);
	//			std::set_difference(up_near_sectors.begin(), up_near_sectors.end(), intersection_sectors.begin(), intersection_sectors.end(), std::back_inserter(new_difference_sectors));
	//			new_difference_sectors.shrink_to_fit();

	//			// 기준 - 이동 후
	//			old_difference_sectors.reserve(11);
	//			std::set_difference(near_sectors.begin(), near_sectors.end(), intersection_sectors.begin(), intersection_sectors.end(), std::back_inserter(old_difference_sectors));
	//			old_difference_sectors.shrink_to_fit();
	//		}
	//	}
	//}

	printf("init map... Ok\n ");
	printf("MMOSimulator Ready... Ok\n ");
}

MMOZone::~MMOZone()
{
}


void MMOZone::accept_actor(MMOActor* actor)
{
	this->actors.insert(std::make_pair(actor->get_id(), actor));
	actor->set_zone(this);

	//MMOSector* dest_sector = &sectors[actor->get_y()][actor->get_x()];
	//dest_sector->accept_actor(actor->get_session_id(), actor);
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
	for ( auto& iter : actors )
	{
		MMOActor* actor = iter.second;
		actor->simulate();
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

MMOSector* MMOZone::get_sector(int y, int x)
{
	//return &sectors[y][x];
}

std::map<int16_t, MMOActor*>& MMOZone::get_actors()
{
	return actors;
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

	sc_packet_leave leave_payload;
	leave_payload.header.length = sizeof(sc_packet_leave);
	leave_payload.header.type = S2C_LEAVE;
	leave_payload.id = actor->get_id();

	MMOServer* server = MMOSimulator::get_instance().get_server();
	for ( auto& iter : actors )
	{
		MMOActor* other = iter.second;
		c2::Packet* leave_packet = c2::Packet::alloc();
		leave_packet->write(&leave_payload, sizeof(sc_packet_leave));
		server->send_packet(other->get_session_id(), leave_packet);
	}
}

