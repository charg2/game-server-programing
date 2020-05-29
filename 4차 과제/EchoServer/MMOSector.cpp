#include "../C2Server/C2Server/pre_compile.h"
#include "MMOActor.h"
#include "MMOSector.h"
#include "MMOZone.h"

MMOSector::MMOSector() 
 : index_x {}, index_y{},
has_obstacle{},
f{}, g{}, h{}
{

	InitializeSRWLock(&lock);
}


MMOSector::~MMOSector()
{
}


void MMOSector::accept_actor(uint64_t session_idx, MMOActor* in_actor)
{
	AcquireSRWLockExclusive( &lock );

	actors.insert(std::make_pair((uint16_t)session_idx, in_actor));
	in_actor->current_sector =this;

	ReleaseSRWLockExclusive(&lock);
}

bool MMOSector::leave_actor(uint64_t session_idx)
{
	auto ret = actors.erase((uint16_t)session_idx);

	return false;
}

void MMOSector::calculate_near_sectors()
{
	using namespace c2::constant;

	for (int y = index_y - 1; y <= index_y + 1; ++y)
	{
		for (int x = index_x -1; x <= index_x + 1; ++x)
		{
			MMOSector* cur_sector;
			if (0 <= y && y <= index_y)
			{
				if (0 <= x && x <= index_x)
				{
					cur_sector = &zone->sectors[y][x];
					this->near_sectors.push_back(cur_sector);
				}
			}
		}
	}

	this->min_x = index_x * FOV_WIDTH;
	this->max_x = min_x + (FOV_WIDTH - 1);
	this->min_y = index_y * FOV_HEIGHT;
	this->max_y = min_y + (FOV_HEIGHT - 1);

	if (max_y > MAP_HEIGHT - 1)
	{
		max_y = MAP_HEIGHT - 1;
	}
	if (max_x > MAP_WIDTH - 1)
	{
		max_x = MAP_WIDTH - 1;
	}

	// 맨마지막에 추가.
	near_sectors.shrink_to_fit();

	//// 좌표가 들어오면 
	//for (int y = min_y; y <= max_y; ++y)
	//{
	//	int count = 0;
	//	for (int x = min_x; x <= max_x; ++x)
	//	{
	//		int l_min_y = max(y - FOV_HALF_HEIGHT, 0), l_min_x = max(x - FOV_HALF_WIDTH, 0);
	//		int l_max_y = min(y + FOV_HALF_HEIGHT, FOV_HEIGHT - 1), l_max_x = min(x + FOV_HALF_WIDTH, FOV_HALF_WIDTH - 1);


	//		MMOSector* l_sectors[4];
	//		l_sectors[0] = zone->get_sector(l_max_y, l_max_x);
	//		l_sectors[1] = zone->get_sector(l_max_y, l_min_x);
	//		l_sectors[2] = zone->get_sector(l_min_y, l_max_x);
	//		l_sectors[3] = zone->get_sector(l_min_y, l_min_x);

	//		near_sector_table[y][x].count = 0;
	//		for (int out = 0; out < 4; ++out)
	//		{
	//			for (int in = 0; in < out; ++in)
	//			{
	//				if ( l_sectors[out] == l_sectors[in] )
	//				{

	//				}
	//				else
	//				{

	//				}
	//			}

	//		}
	//	}
	//}



}

void MMOSector::broadcaset() {}

