#pragma once
#include "MMOSector.h"
#include <map>
#include <vector>

class MMOACtor; 
class MMOZone //
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

	MMOSector* get_sector(int y, int x);
	std::map<int16_t, MMOActor*>&	get_actors();

private:
	std::map<int16_t, MMOActor*>	actors;
	std::vector<int16_t>			garbages;
};

