#include "script_api.h"

int l2c_get_npc_pos_x(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1); // 제일 최근에 스택에 넣은 값.
	lua_pop(vm, 2);
	int x = g_npc_manager->get_npc(npc_id)->x;
	lua_pushnumber(vm, x);
	return 1;
}

int l2c_get_npc_pos_y(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1);		// 제일 최근에 스택에 넣은 값.
	lua_pop(vm, 2);								// 
	int y = g_npc_manager->get_npc(npc_id)->y;	// 
	lua_pushnumber(vm, y);
	return 1;
}

int l2c_get_npc_target_id(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1);		// 제일 최근에 스택에 넣은 값.
	lua_pop(vm, 2);								// 
	int y = g_npc_manager->get_npc(npc_id)->y;	// 
	lua_pushnumber(vm, y);
	return 0;
}

int l2c_send_chatting_to_target(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1);		// 제일 최근에 스택에 넣은 값.
	lua_pop(vm, 2);			
	
	MMONpc* npc = g_npc_manager->get_npc(npc_id);	// 
	
	

	return 1;
}

int l2c_npc_move_to_anywhere(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1);		// 제일 최근에 스택에 넣은 값.
	lua_pop(vm, 2);								// 
	MMONpc* npc = g_npc_manager->get_npc(npc_id);	// 
	npc->move_to_anywhere();

	return 1;
}

int l2c_npc_go_sleep(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1);		// 제일 최근에 스택에 넣은 값.
	lua_pop(vm, 2);								// 
	MMONpc* npc = g_npc_manager->get_npc(npc_id);	// 
	npc->is_active = false;

	return 1;
}

