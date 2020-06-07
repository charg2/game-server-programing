#include "pre_compile.h"
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
	//in_actor->current_sector =this;

	ReleaseSRWLockExclusive(&lock);
}

bool MMOSector::leave_actor(uint64_t session_idx)
{
	auto ret = actors.erase((uint16_t)session_idx);

	return false;
}

void MMOSector::calculate_serctor_idx()
{
	using namespace c2::constant;
	this->sector_min_x = index_x * SECTOR_WIDTH;
	this->sector_max_x = sector_min_x + (SECTOR_WIDTH - 1);
	this->sector_min_y = index_y * SECTOR_HEIGHT;
	this->sector_max_y = sector_min_y + (SECTOR_HEIGHT - 1);

	// 섹터안에서 움직이는 경우가 훨씬 많다.
	int effective_map_min_x = sector_min_x + FOV_HALF_WIDTH;
	int effective_map_max_x = sector_max_x - FOV_HALF_WIDTH;
	int effective_map_min_y = sector_min_y + FOV_HALF_WIDTH;
	int effective_map_max_y = sector_max_y - FOV_HALF_WIDTH;

	if (sector_max_y > MAP_HEIGHT - 1)
	{
		sector_max_y = MAP_HEIGHT - 1;
	}
	if (sector_max_x > MAP_WIDTH - 1)
	{
		sector_max_x = MAP_WIDTH - 1;
	}


	for (int y = sector_min_y; y <= sector_max_y; ++y)
	{
		// 좌표가 들어오면 
		// 중복이 많아서 제거하긴 해야함.
		MMONear* near_cache[12]{};
		int		 near_cache_cnt{};

		for (int x = sector_min_x; x <= sector_max_x; ++x)
		{
			if (x == sector_max_x && y == sector_max_y )
			{
				int n = 0;
			}

			int l_min_y = max(y - FOV_HALF_HEIGHT, 0), l_min_x = max(x - FOV_HALF_WIDTH, 0);
			int l_max_y = min(y + FOV_HALF_HEIGHT, MAP_HEIGHT - 1), l_max_x = min(x + FOV_HALF_WIDTH, MAP_WIDTH - 1);

			MMOSector* l_sectors[4];
			l_sectors[0] = zone->get_sector(l_max_y, l_max_x);
			l_sectors[1] = zone->get_sector(l_max_y, l_min_x);
			l_sectors[2] = zone->get_sector(l_min_y, l_max_x);
			l_sectors[3] = zone->get_sector(l_min_y, l_min_x);

			//near_sector_table[y - sector_min_y][x - sector_min_x] = new MMONear();
			MMONear* near_sect = new MMONear{ };

			for ( MMOSector* lsector : l_sectors) // 찾은 섹터들중에서 
			{
				int same_cnt = 0;
				for (int in = 0; in < near_sect->count; ++in)	// 배열에서 중복검사.
				{
					// 기존 배열에 있던 녀석중에서 같은녀석이 있다? 그럼 무시;
					if (near_sect->sectors[in] == lsector)
					{
						same_cnt += 1;
					}
				}

				if (same_cnt == 0) // 그리고 자신도 아니라면 
				{
					near_sect->sectors[near_sect->count] = lsector; // 추가함
					near_sect->count += 1;
				}
			}
			if (near_sect->count == 0)
			{
				delete near_sect;
				near_sector_table[y - sector_min_y][x - sector_min_x] = nullptr;
			}
			else
			{
				bool has_same{};
				for ( MMONear* the_near : near_cache)
				{
					if (the_near != nullptr &&  (*the_near  == *near_sect) == true)
					{
						near_sector_table[y - sector_min_y][x - sector_min_x] = the_near;
						has_same = true;
						break;
					}
				}
				if (has_same == true)	// 풀내에 이미 있다면 지워줌.
				{
					delete near_sect;	// 사실 
				}
				else
				{
					near_sector_table[y - sector_min_y][x - sector_min_x] = near_sect;
					near_cache[near_cache_cnt] = near_sect;
					near_cache_cnt += 1;
				}
			}
		}
	}
}

void MMOSector::broadcaset() {}

const MMONear* const MMOSector::get_near(int y, int x) const
{
	return this->near_sector_table[y - sector_min_y][ x- sector_min_x];
}

