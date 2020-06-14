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


struct MMOSector;
struct MMOActor;
struct MMOZone;


struct MMONpc
{
	void update_entering_actor(MMOActor* actor);
	void update_leaving_actor(MMOActor* actor);

	void prepare_virtual_machine();
	void move();
	void move_to_anywhere();
	void send_chatting_to_actor(int32_t actor_id, char* message);
	
	int16_t		hp;
	int16_t		max_hp;
	int32_t		x, y;
	char		name[50];
	uint64_t	id;
	MMOSector*	current_sector;
	MMOActor*	target;
	MMOZone*	zone;
	uint64_t	is_active;
	int32_t		target_id;

	std::unordered_map<int32_t, MMOActor*>	view_list;
	SRWLOCK									lock;

	lua_State*	lua_vm;
	SRWLOCK		vm_lock;
};

