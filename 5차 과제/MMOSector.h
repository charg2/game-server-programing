#pragma once

// ������ ������ ������ ������.

#include <vector>
#include <map>
#include <set>
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
	std::map<int16_t, MMOActor*>	actors;
	std::set<int32_t>				npcs;
	MMONear*						near_sector_table[SECTOR_HEIGHT][SECTOR_WIDTH];
	MMOZone*						zone;
	int								index_x, index_y;
	int								sector_min_x, sector_max_x;
	int								sector_min_y, sector_max_y;
	SRWLOCK							lock;
	bool							has_obstacle; // ��ֹ� �����ϴ���;
	int								f;
	int								g;
	int								h;
};


