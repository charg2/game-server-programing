#pragma once
#include <Windows.h>
#include <cstdint>
#include <map>


extern "C"
{
#include "lua\lua.h"
#include "lua\lauxlib.h"
#include "lua\lualib.h"
}

struct MMOSector;
struct MMOActor;
struct MMOZone;
struct MMONpc
{
	void update_entering_actor(MMOActor* actor);
	void update_leaving_actor(MMOActor* actor);

	void preare_vm();
	void move();

	int16_t		hp;
	int16_t		max_hp;
	int32_t		x, y;
	char		name[50];
	uint64_t	id;
	MMOSector*	current_sector;
	MMOActor*	target;
	MMOZone*	zone;
	uint64_t	is_active;

	std::map<int32_t, MMOActor*>	view_list;
	SRWLOCK							lock;

	lua_State*	lua_vm;
	SRWLOCK		vm_lock;

};

