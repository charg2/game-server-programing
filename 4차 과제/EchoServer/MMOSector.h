#pragma once

#include <vector>
#include <map>

class MMOActor;
class MMOZone;
class MMOSector 
{
public:
	MMOSector();
	~MMOSector();

	void simulate();
	void accept_actor(uint64_t session_idx, MMOActor* in_actor);
	bool leave_actor(uint64_t session_idx);
	void calculate_near_sectors();


	bool get_has_obstacle();
	void set_position(int y, int x);
	void set_zone(MMOZone* zone);

	std::vector<MMOSector*>& get_near_sectors();
	std::map<uint16_t, MMOActor*>& get_actors();
	int get_x();
	int get_y();

	//std::vector<MMOSector*>&	get_old_difference_sectors(c2::enumeration::NearDirection direction);
	//std::vector<MMOSector*>&	get_intersection_sectors(c2::enumeration::NearDirection direction);
	//std::vector<MMOSector*>&	get_new_difference_sectors(c2::enumeration::NearDirection direction);

private:
	//std::map<uint16_t, MMOActor*> actors;
	//int position_x;
	//int position_y;

	//bool has_obstacle; // 장애물 포함하는지;

	//int f;
	//int g;
	//int h;
	
	//std::vector<MMOSector*>		near_sectors; // for broadcast

	//std::vector<MMOSector*>		old_difference_sectors[c2::enumeration::NEAR_MAX]; // prev_sectors - cur_sectors
	//std::vector<MMOSector*>		intersection_sectors[c2::enumeration::NEAR_MAX]; // prev_sectors - cur_sectors
	//std::vector<MMOSector*>		new_difference_sectors[c2::enumeration::NEAR_MAX]; // cur_sectors - prev_sectors

	MMOZone* zone;
};


