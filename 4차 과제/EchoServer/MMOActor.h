#pragma once

// content
#include "../C2Server/C2Server/enviroment.h"
#include "../C2Server/C2Server/protocol.h"
#include <cstdint>
#include <atomic>
#include <map>

struct	MMOZone;
struct	MMOSector;
class	MMOServer;
class	MMOSession;

enum ActorStatus { ST_FREE, ST_ALLOCATED, ST_ACTIVE };

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

	void send_enter_packet(MMOActor* other);
	void send_move_packet(MMOActor* other);
	void send_leave_packet(MMOActor* other);
	void sned_chat_packet(MMOActor* other);
	void send_login_ok_packet();

public:
	int32_t				x, y;
	char				name[50];
	int16_t				hp;
	int16_t				level;
	int32_t				current_exp;
	int32_t				levelup_exp;
	int8_t				direction;

	std::atomic<ActorStatus> status = ST_FREE;

	uint64_t			session_id;
	unsigned			last_move_time;
	MMOSession* const	session;
	MMOZone*			zone;


	MMOServer*			server;
	int32_t				sector_x, sector_y;
	MMOSector*			current_sector; 

	std::map<int32_t, MMOActor*>	view_list;
	SRWLOCK				lock;
};

