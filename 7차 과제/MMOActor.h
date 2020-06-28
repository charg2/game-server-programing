#pragma once

// content
#include "protocol.h"
#include <cstdint>
#include <atomic>
#include <unordered_map>
#include <unordered_set>

struct	MMOZone;
struct	MMOSector;
class	MMOServer;
class	MMOSession;
struct	MMONPC;
struct  LoadActorTask;
struct  CreateActorTask;
enum ActorStatus { ST_FREE, ST_ALLOCATED, ST_ACTIVE };

using namespace c2::enumeration;
struct MMOActor
{
public:
	MMOActor(MMOSession* owner);
	~MMOActor();

	void enter_sector(int32_t x, int32_t y);
	void attack();
	void reset();
	void reset_when_respawn();
	void exit();
	void respawn();
	void move(char direction);

	// 체력이 깍이면 do_auto_recovery
	void reset_data(const LoadActorTask* task);
	void reset_data_when_creation(const CreateActorTask* task);

	bool is_near(MMOActor* actor);
	bool is_near(MMONPC* npc);

	void increase_exp(int32_t exp);
	void decrease_exp(int32_t exp);
	void increase_hp(int32_t hp);
	void decrease_hp(MMONPC* npc, int32_t hp);

	void get_login_packet_info(sc_packet_login_ok& out_packet);

	int16_t get_id();

	void send_enter_packet(MMOActor* other);
	void send_enter_packet(MMOActor* other, c2::Packet* enter_packet);
	void send_enter_packet(MMONPC* other);
	void send_enter_packet_without_updating_viewlist(MMOActor* other);
	void send_enter_packet_without_updating_viewlist(MMONPC* other);

	void send_move_packet(MMOActor* other);
	void send_move_packet(MMOActor* other, c2::Packet* enter_packet);
	void send_move_packet(MMONPC* other);

	void send_leave_packet(MMOActor* other);
	void send_leave_packet(MMOActor* other, c2::Packet* enter_packet);
	void send_leave_packet(MMONPC* other);
	void send_leave_packet_without_updating_viewlist(MMOActor* other);
	void send_leave_packet_without_updating_viewlist(MMONPC* other);

	void sned_chat_packet(MMOActor* other);
	void send_login_ok_packet();

	void send_stat_change();

	void wake_up_npc(MMONPC* npc);

public:
	int32_t							x, y;
	wchar_t							name[50];
	int16_t							hp;
	int16_t							level;
	int32_t							current_exp;
	int32_t							levelup_exp;

	std::atomic<ActorStatus>		status = ST_FREE;

	uint64_t						session_id;
	uint64_t						user_id;
	unsigned						last_move_time;
	MMOZone*						zone;

	MMOServer*						server;
	MMOSector*						current_sector; 
	MMOSession* const				session;
	bool							is_alive;

	std::unordered_map<int32_t, MMOActor*>	view_list;
	std::unordered_set<int32_t>				view_list_for_npc;
	SRWLOCK							lock;
};

