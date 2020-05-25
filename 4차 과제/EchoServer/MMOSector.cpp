#include "../C2Server/C2Server/pre_compile.h"
#include "MMOActor.h"
#include "MMOSector.h"
#include "MMOZone.h"

MMOSector::MMOSector() 
 : position_x {}, position_y{},
has_obstacle{},
f{}, g{}, h{}
{
}

MMOSector::~MMOSector()
{
}

void MMOSector::simulate()
{
	for ( auto& it : actors)
	{
		it.second->simulate();
	}
}

void MMOSector::accept_actor(uint64_t session_idx, MMOActor* in_actor)
{
	actors.insert(std::make_pair((uint16_t)session_idx, in_actor));

	in_actor->set_current_sector(this);
}

bool MMOSector::leave_actor(uint64_t session_idx)
{
	auto ret = actors.erase((uint16_t)session_idx);

	return false;
}

void MMOSector::calculate_near_sectors()
{
	// x 범위 구해주고
	int16_t min_x = max(0, this->position_x - (c2::constant::BROADCAST_WIDTH / 2));
	int16_t max_x = min(c2::constant::MAP_WIDTH, this->position_x + (c2::constant::BROADCAST_WIDTH / 2));

	// y 범위 구해주고
	int16_t min_y = max(0, this->position_y - (c2::constant::BROADCAST_HEIGHT / 2));
	int16_t max_y = min(c2::constant::MAP_HEIGHT, this->position_y + (c2::constant::BROADCAST_HEIGHT / 2));

	for (int y = min_y; y <= max_y; ++y)
	{
		for (int x = min_x; x <= max_x; ++x)
		{
			MMOSector* cur_zone = zone->get_sector(y, x);
			
			this->near_sectors.push_back(cur_zone);
		}
	}

	// 맨마지막에 추가.
	near_sectors.shrink_to_fit();

	std::sort(near_sectors.begin(), near_sectors.end());
}

bool MMOSector::get_has_obstacle()
{
	return has_obstacle;
}

void MMOSector::set_position(int y, int x)
{
	this->position_x = x;
	this->position_y = y;
}

void MMOSector::set_zone(MMOZone* zone)
{
	this->zone = zone;
}

std::vector<MMOSector*>& MMOSector::get_near_sectors()
{
	return this->near_sectors;
}

std::map<uint16_t, MMOActor*>& MMOSector::get_actors()
{
	return actors;
}

int MMOSector::get_x()
{
	return position_x;
}

int MMOSector::get_y()
{
	return position_y;
}

std::vector<MMOSector*>& MMOSector::get_old_difference_sectors(c2::enumeration::NearDirection direction)
{
	return this->old_difference_sectors[direction];
}

std::vector<MMOSector*>& MMOSector::get_intersection_sectors(c2::enumeration::NearDirection direction)
{
	return this->intersection_sectors[direction];
}

std::vector<MMOSector*>& MMOSector::get_new_difference_sectors(c2::enumeration::NearDirection direction)
{
	return this->new_difference_sectors[direction];
}


