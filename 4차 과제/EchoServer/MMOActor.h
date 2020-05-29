#pragma once

// content
#include "../C2Server/C2Server/enviroment.h"
#include "../C2Server/C2Server/protocol.h"
#include <cstdint>
#include <set>

struct MMOZone;
struct MMOSector;
class MMOSession;


using namespace c2::enumeration;
struct MMOActor
{
public:
	MMOActor(MMOSession* owner);
	~MMOActor();

	void enter_sector(int32_t x, int32_t y);
	void move(int8_t direction);
	void attack();
	void reset();
	void exit();

	bool is_near(MMOActor* actor);
	void get_login_packet_info(sc_packet_login_ok& out_packet);

	int16_t get_id();

public:
	int32_t				x, y;
	char				name[50];
	int16_t				hp;
	int16_t				level;
	int32_t				current_exp;
	int32_t				levelup_exp;
	int8_t				direction;

	uint64_t			session_id;
	unsigned			last_move_time;
	MMOSession* const	session;
	MMOZone*			zone;

	int32_t				sector_x, sector_y;
	MMOSector*			current_sector; 

	std::set<int16_t>	view_list;
	SRWLOCK				lock;
};

