#pragma once
#include <map>
#include <vector>

#include "MMOSector.h"
#include "contents_enviroment.h"

using namespace c2::enumeration;
class MMOServer;
struct MMOACtor;
struct MMOZone //
{
public:
	MMOZone();
	MMOZone(const MMOZone& other) = delete;
	MMOZone(MMOZone&& other) = delete;
	~MMOZone();

	void enter_actor(MMOActor* actor);		//  for login
	void leave_actor(MMOActor* actor);		//  for logout
	void move_sector(MMOActor* actor, MMOSector* start, MMOSector* end); // move to
	
	MMOSector* get_sector(int32_t y, int32_t x);
	MMOSector* get_sector(MMOActor* actor);

public:
	MMOSector						sectors[MAP_NAX_HEIGHT_INDEX][MAP_NAX_WIDTH_INDEX];

	short							postion_height_mapling_table[MAP_HEIGHT]; // only read after init
	short							postion_width_mapling_table[MAP_WIDTH]; // only read after init

	MMOServer*						server;// only read after init
};

