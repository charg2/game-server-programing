#pragma once

#include <cstdint>

class MMOZone;
class MMOSimulator;
class MMOSector;

class MMOActor
{
	MMOActor();
	~MMOActor();

	void move_to(int32_t x, int32_t y);
	void reset();


private:
	// name 
	// text

	int32_t			x, y;
	char			name[c2::constant::MAX_ID_LEN];
	uint64_t		session_id;
	MMOSector*		current_sector;
	MMOSector*		prev_sector;
	MMOZone*		zone;
	MMOSimulator*	simulator;
};

