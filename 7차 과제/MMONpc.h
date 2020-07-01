#pragma once
#include <Windows.h>
#include <cstdint>
#include <unordered_map>


extern "C"
{
#include "lua\lua.h"
#include "lua\lauxlib.h"
#include "lua\lualib.h"
}


enum NPCState
{
	NPC_SLEEP = 0,
	NPC_WORKING = 1, // ±ú¿ì±â¸¸
};

enum NPCType
{
	NT_PEACE_FIXED,		// 0000		
	NT_PEACE_ROAMING,	// 0001
	NT_COMBAT_FIXED,	// 0010
	NT_COMBAT_ROAMING,	// 0011

	NT_MAX
};
struct MMOSector;
struct MMOActor;
struct MMOZone;


struct MMONPC
{
	void update_entering_actor(MMOActor* actor);
	void update_leaving_actor(MMOActor* actor);

	void initialize_vm_and_load_data();
	void move_to_anywhere();
	void reset();
	void respawn();
	void attack_for_peace();
	void die();
	void initialize(size_t id_base);
	void send_chatting_to_actor(int32_t actor_id, wchar_t* message);
	
	void update_for_fixed_peace();
	void update_for_peace();
	void update_for_fixed_combat();
	void update_for_combat();

	void set_target(MMOActor* actor);

	void decrease_hp(MMOActor* actor, int32_t damage);
	
	bool is_in_attack_range(MMOActor* actor);

	bool is_in_attack_scope(MMOActor* actor);
	void send_chat_packet(const wchar_t* msg);
	void send_chat_packet_to_target(MMOActor* target, const wchar_t* msg);

	int16_t		hp;
	int16_t		max_hp;
	int16_t		level;
	int32_t		x, y;
	int32_t		initial_x, initial_y;
	wchar_t		name[50];
	uint64_t	id;
	int8_t		is_alive;
	int8_t		has_target;

	uint64_t	state; 
	MMOSector*	current_sector;
	MMOActor*	target;
	MMOZone*	zone;
	uint64_t	is_active;
	int32_t		target_id;
	int32_t		exp;

	NPCType		type;
	int			dmg;

	std::unordered_map<int32_t, MMOActor*>	view_list;
	SRWLOCK									lock;

	lua_State*	lua_vm;
	SRWLOCK		vm_lock;
};

