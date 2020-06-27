#include "script_api.h"
#include "util/TimeScheduler.h"
//#include "MMOServer.h"

int l2c_get_npc_pos_x(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1); // ���� �ֱٿ� ���ÿ� ���� ��.
	lua_pop(vm, 2);
	int x = g_npc_manager->get_npc(npc_id)->x;
	lua_pushnumber(vm, x);
	return 1;
}

int l2c_get_npc_pos_y(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1);		// ���� �ֱٿ� ���ÿ� ���� ��.
	lua_pop(vm, 2);								// 
	int y = g_npc_manager->get_npc(npc_id)->y;	// 
	lua_pushnumber(vm, y);
	return 1;
}

int l2c_get_npc_target_id(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1);		// ���� �ֱٿ� ���ÿ� ���� ��.
	lua_pop(vm, 2);								// 
	int y = g_npc_manager->get_npc(npc_id)->y;	// 
	lua_pushnumber(vm, y);
	return 0;
}

int l2c_send_chatting_to_target(lua_State* vm)
{
	int actor_id = (int)lua_tonumber(vm, -3);		// ���� �ֱٿ� ���ÿ� ���� ��.
	int my_id = (int)lua_tonumber(vm, -2);		// ���� �ֱٿ� ���ÿ� ���� ��.
	char* msg = (char*)lua_tostring(vm, -1);


	wchar_t w_msg_buffer[81]; // ���� �������� ä���� �ִ� 80����Ʈ��.
	MultiByteToWideChar(CP_ACP, 0, msg, -1, w_msg_buffer, _countof(w_msg_buffer));

	lua_pop(vm, 4);			 // �������� �Ӹ� �س��� ��.
	
	MMONPC* npc = g_npc_manager->get_npc(my_id);

	npc->send_chatting_to_actor(actor_id, w_msg_buffer);

	return 1;
}

int l2c_npc_move_to_anywhere(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1);		// ���� �ֱٿ� ���ÿ� ���� ��.
	lua_pop(vm, 2);								// 

	MMONPC* npc = g_npc_manager->get_npc(npc_id);	// 
	
	//printf("1");
	if (npc->is_active == NPC_SLEEP)
	{
		//printf("2");
		if (NPC_SLEEP == InterlockedExchange(&npc->is_active, NPC_WORKING))	// ���� ���°� �ڰ� �־��ٸ� �ƿ�.
		{
			//printf("3\n");
			local_timer->push_timer_task(npc->id, TTT_NPC_SCRIPT2, 1000, 0);
		}
	}

	return 1;
}

int l2c_npc_go_sleep(lua_State* vm)
{
	int npc_id = (int)lua_tonumber(vm, -1);		// ���� �ֱٿ� ���ÿ� ���� ��.
	lua_pop(vm, 2);								// 
	MMONPC* npc = g_npc_manager->get_npc(npc_id);	// 
	npc->is_active = false;

	return 1;
}

