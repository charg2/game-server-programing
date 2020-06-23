#pragma once

// 기존의 섹터의 개념을 변경함.

#include <bitset>
#include <unordered_map>
#include <unordered_set>
#include "contents_enviroment.h"
#include "MMONear.h"

struct MMOActor;
struct MMOZone;
struct MMONpc;
using namespace c2::constant;

struct MMOSector 
{
	MMOSector();
	~MMOSector();

	void accept_actor(uint64_t session_idx, MMOActor* in_actor);
	bool leave_actor(uint64_t session_idx);

	void calculate_serctor_idx();
	void broadcaset();

	const MMONear* const get_near(int y, int x) const;

///////////////////
	std::unordered_map<int16_t, MMOActor*>	actors;
	std::unordered_set<int32_t>				npcs;
	MMONear*								near_sector_table[SECTOR_HEIGHT][SECTOR_WIDTH];
	MMOZone*								zone;
	int										index_x, index_y;
	int										sector_min_x, sector_max_x;
	int										sector_min_y, sector_max_y;
	SRWLOCK									lock;
	bool									has_obstacle; // 장애물 포함하는지;
	int										f;
	int										g;
	int										h;


//////////
	static inline std::bitset<c2::constant::MAP_WIDTH>	obstacle_table[c2::constant::MAP_HEIGHT];
};


