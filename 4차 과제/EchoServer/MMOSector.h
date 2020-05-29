#pragma once

// 기존의 섹터의 개념을 변경함.

#include <vector>
#include <map>
#include "MMONear.h"

struct MMOActor;
struct MMOZone;

using namespace c2::constant;

struct MMOSector 
{
	MMOSector();
	~MMOSector();

	void accept_actor(uint64_t session_idx, MMOActor* in_actor);
	bool leave_actor(uint64_t session_idx);

	void calculate_near_sectors();
	void broadcaset();

///////////////////
	
	std::map<uint16_t, MMOActor*>	actors;
	std::vector<MMOSector*>			near_sectors; // for broadcast
	MMONear							near_sector_table[BROADCAST_HEIGHT][BROADCAST_WIDTH];

	MMOZone*	zone;
	int			index_x, index_y;
	int			min_x, max_x;
	int			min_y, max_y;
	SRWLOCK		lock;
	bool		has_obstacle; // 장애물 포함하는지;
	int			f;
	int			g;
	int			h;
};


