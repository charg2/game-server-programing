#pragma once
#include "MMOSector.h"
#include <map>
#include <vector>


class MMOServer;
struct MMOACtor;
struct MMOZone //
{
public:
	MMOZone();
	MMOZone(const MMOZone& other) = delete;
	MMOZone(MMOZone&& other) = delete;
	~MMOZone();

	void accept_actor(MMOActor* actor);
	void release_actor(MMOActor* actor);
	void erase_session(uint64_t session_id);

	void broadcaset();
	MMOSector* get_sector(int32_t y, int32_t x);
	MMOSector* calc_near_sector(int32_t y, int32_t x);


public:
	MMOSector						sectors[c2::constant::MAP_HEIGHT][c2::constant::MAP_WIDTH];

	short							postion_height_mapling_table[c2::constant::MAP_HEIGHT]; // only read after init
	//std::map<int16_t, MMOActor*>	actors;
	short							postion_width_mapling_table[c2::constant::MAP_WIDTH]; // only read after init
	MMOServer*						server;// only read after init
	SRWLOCK							lock;
};

