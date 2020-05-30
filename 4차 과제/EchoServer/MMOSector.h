#pragma once

#include <vector>
#include <map>

class MMOActor;
struct MMOZone;
struct MMONear;
struct MMOSector 
{
public:
	MMOSector();
	~MMOSector();

	void simulate();
	void accept_actor(uint64_t session_idx, MMOActor* in_actor);
	bool leave_actor(uint64_t session_idx);
	void calculate_near_sectors();
	void calculate_serctor_idx();
	bool get_has_obstacle();
	void set_zone(MMOZone* zone);
	void broadcaset();
	MMONear*  get_near(int y, int x) const;


public:
	std::map<uint16_t, MMOActor*>	actors;
	MMOZone*								zone;
	int										index_x, index_y;
	int										sector_min_x, sector_max_x;
	int										sector_min_y, sector_max_y;
	bool									has_obstacle; // 장애물 포함하는지;
	MMONear*								near_sector_table[SECTOR_HEIGHT][SECTOR_WIDTH];
	int										f;
	int										g;
	int										h;
};


