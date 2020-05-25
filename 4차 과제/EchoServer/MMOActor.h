#pragma once

// content
#include "../C2Server/C2Server/enviroment.h"
#include "../C2Server/C2Server/protocol.h"
#include <cstdint>
class MMOZone;
class MMOSimulator;
class MMOSector;
class MMOSession;


using namespace c2::enumeration;
class MMOActor
{
public:
	MMOActor(MMOSession* owner);
	~MMOActor();

	void enter_sector(int32_t x, int32_t y);
	void move_to(int32_t dest_x, int32_t dest_y);
	void move(int8_t direction);

	void simulate();
	void attack();
	void reset();

	void set_current_sector(MMOSector* sector);
	void set_prev_sector(MMOSector* sector);
	void set_zone(MMOZone* zone);
	void set_name(char* name_ptr);
	void set_move_time(unsigned time);
	void set_state(c2::enumeration::MMOActorState state);

	uint64_t get_session_id();
	MMOSector* get_current_sector();
	MMOSector* get_prev_sector();
	int16_t get_id();
	int32_t get_x();
	int32_t get_y();
	unsigned get_move_time();

	void get_login_packet_info(sc_packet_login_ok& out_packet);

private:
	// actor_state
	int32_t				x, y;
	char				name[50];
	int16_t				hp;
	int16_t				level;
	int32_t				current_exp;
	int32_t				levelup_exp;
	int8_t				direction;
	MMOActorState		state;
	uint64_t			session_id;
	unsigned			last_move_time;
	MMOSector*			current_sector; // in dispatch()
	MMOSector*			prev_sector;	// in simulate()
	MMOZone*			zone;
	MMOSimulator*		simulator;
	MMOSession* const	session;
};

