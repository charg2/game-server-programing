#pragma once
#include "MMOSector.h"
#include <map>
#include <vector>

using namespace c2::constant;

struct	MMOACtor;
class	MMOServer;
struct MMOZone //
{
public:
	MMOZone();
	MMOZone(const MMOZone& other) = delete;
	MMOZone(MMOZone&& other) = delete;
	~MMOZone();

	void accept_actor(MMOActor* actor);
	void release_actor(MMOActor* actor);

	void dispatch_network();
	void simulate_interaction();

	void broadcaset_all();
	void broadcaset_nearby_others();
	void add_garbage(int16_t id);

	void erase_session(uint64_t session_id);

	MMOSector* get_sector(int32_t y, int32_t x);
	
public:
	MMOServer*						server;
	MMOSector						sectors[MAP_NAX_HEIGHT_INDEX][MAP_NAX_WIDTH_INDEX];
	int								index_y, index_x;
	short							postion_height_mapling_table[c2::constant::MAP_HEIGHT]; 
	short							postion_width_mapling_table[c2::constant::MAP_WIDTH]; 

	std::map<int16_t, MMOActor*>	actors;
	std::vector<int16_t>			garbages;
};

