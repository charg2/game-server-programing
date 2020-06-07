#pragma once
#include "MMONpcManager.h"

extern "C"
{
#include "lua\lua.h"
#include "lua\lauxlib.h"
#include "lua\lualib.h"
}

int l2c_get_npc_pos_x(lua_State* vm);
int l2c_get_npc_pos_y(lua_State* vm);
int l2c_get_npc_target_id(lua_State* vm);
int l2c_send_chatting_to_target(lua_State* vm);
int l2c_npc_move_to_anywhere(lua_State* vm);
int l2c_npc_go_sleep(lua_State* vm);
