
#include "../C2Server/C2Server/pre_compile.h"
#include "../C2Server/C2Server/enviroment.h"
#include "function.h"
#include "MMONpc.h"
#include "MMOActor.h"
#include "MMONpcManager.h"
#include "MMOZone.h"
#include "MMONear.h"

//#include "../C2Server/C2Server/util/TimeScheduler.h"
//#include "MMOActor.h"

using namespace c2::constant;

void MMONpc::preare_vm()
{
	lua_vm = luaL_newstate();
	luaL_openlibs(lua_vm);

	int error = luaL_loadfile(lua_vm, "npc.lua");
	error = lua_pcall(lua_vm, 0, 0, 0);
	if (error != 0) 
		printf( "lua error %s \n", lua_tostring(lua_vm, -1));

	lua_getglobal(lua_vm, "set_npc_id");
	lua_pcall(lua_vm, 0, 0, 0);

	lua_getglobal(lua_vm, "set_npc_id");
	lua_pushnumber(lua_vm, id);
	lua_pcall(lua_vm, 1, 1, 0);
	lua_pop(lua_vm, 1);


	//lua_register(L, "API_send_message", API_SendMessage);
	//lua_register(L, "API_get_x", API_get_x);
	//lua_register(L, "API_get_y", API_get_y);
	//lua_register(L, "API_get_y", API_get_cur_state);
}

void MMONpc::move()
{
	//int8_t direction = rand
	// ��ǥ����.. 
	// ��ã�� �� ĭ.
	char direction = fast_rand() % 4;
	bool is_isolated = true;
	int local_y = y;
	int local_x = x;
	int local_actor_id = id;


	is_active = NPC_SLEEP;  // Ÿ �����忡�� �����ؼ� ���� ���� ç �� �ֵ���...

	// ��ֹ� üũ ���.
	switch (direction)
	{
	case D_DOWN:
		if (local_y < MAP_HEIGHT - 1)	local_y++;
		break;
	case D_LEFT:
		if (local_x > 0) local_x--;
		break;
	case D_RIGHT:
		if (local_x < MAP_WIDTH - 1) local_x++;
		break;
	case D_UP:
		if (local_y > 0) local_y--;
		break;
	default:
		size_t* invalid_ptr{}; *invalid_ptr = 0;
		break;
	}

	x = local_x;
	y = local_y;



	MMOSector* new_sector = zone->get_sector(local_y, local_x);			// view_list �ܾ����.
	// ���Ͱ� ����Ǹ� �ٲٱ�.
	if (current_sector != new_sector)
	{
		AcquireSRWLockExclusive(&current_sector->lock); // ���� ���� ������
		current_sector->npcs.erase(id);
		ReleaseSRWLockExclusive(&current_sector->lock);

		// ���� ���� �鰡��.
		AcquireSRWLockExclusive(&new_sector->lock);
		this->current_sector = new_sector;
		new_sector->npcs.insert(id);
		ReleaseSRWLockExclusive(&new_sector->lock);
	}


	// �丮��Ʈ ����.
	AcquireSRWLockExclusive(&lock); // // �� �ɰ�...
	std::map<int32_t, MMOActor*> local_old_view_list = view_list;
	ReleaseSRWLockExclusive(&lock); // // �� �ɰ�...


	// �ֺ��� �Ѹ���.
	MMOSector*		current_sector	= new_sector;
	const MMONear*	nears			= current_sector->get_near(local_y, local_x);
	int				near_cnt		= nears->count;

	
	// �� �ֺ� ������ �ܾ� ����.
	std::map<int32_t, MMOActor*>	local_new_view_list;
	for (int n = 0; n < near_cnt; ++n)
	{
		AcquireSRWLockShared(&nears->sectors[n]->lock); // sector�� �б� ���ؼ� ���� ��� 
		for (auto& actor_iter : nears->sectors[n]->actors)
		{
			//if (actor_iter.second->status != ST_ACTIVE) 		//continue;
			if (actor_iter.second->is_near(this) == true) // �� ��ó�� �´ٸ� ����.
			{
				is_isolated = false;
				local_new_view_list.insert(actor_iter);
			}
		}
		ReleaseSRWLockShared(&nears->sectors[n]->lock);
	}

	/*if (local_new_view_list.size() > 0)
		is_isolated = false;*/


	// broad cast

	///////////////////
	for (auto& new_actor : local_new_view_list)
	{
		if (0 == local_old_view_list.count(new_actor.first))	// �̵��� ���� ���̴� ����.
		{
			this->update_entering_actor(new_actor.second);		// ���� ���̴� ���� ������ NPC���� ������Ʈ ��.

			AcquireSRWLockShared(&new_actor.second->lock);
			if (0 == new_actor.second->view_list_for_npc.count(this->id)) // Ÿ �����忡�� �þ� ó�� �� �Ȱ��.
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_enter_packet(this);
			}
			else // ó�� �Ȱ��
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_move_packet(this);   // ��� �þ� ����Ʈ�� ���� �ִ� ��� �丮��Ʈ�� ������Ʈ �Ѵ�.
			}
		}
		else  // ���� �丮��Ʈ�� �ִ� ������ 
		{
			AcquireSRWLockShared(&new_actor.second->lock);					//
			if (0 != new_actor.second->view_list_for_npc.count(this->id))	// 
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_move_packet(this);
			}
			else	// �̹� ���� ���.
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_enter_packet(this);
			}
		}
	}


	//�þ߿��� ��� �÷��̾�
	for (auto& old_it : local_old_view_list)
	{
		if (0 == local_new_view_list.count(old_it.first))
		{
			this->update_leaving_actor(old_it.second); // npc �þ߿��� ������ �������� ���� ����.

			AcquireSRWLockShared(&old_it.second->lock);
			if (0 != old_it.second->view_list_for_npc.count(this->id))
			{
				ReleaseSRWLockShared(&old_it.second->lock);
				old_it.second->send_leave_packet(this);
			}
			else	// �̹� �ٸ� �����忡�� ������ ���.
			{
				ReleaseSRWLockShared(&old_it.second->lock);
			}
		}
	}


	// �ֺ��� �ƹ��� ���ٸ�?
	if (is_isolated == true)
	{
		// �� ����� �ǵ鿴���� �������� ó������.
		return;
	}
	else // �ִٸ� ������Ʈ ���.
	{
		// is_active ������ �� ������ �־ ������Ʈ ������ ���� ������ ���� ����.
		if (InterlockedExchange(&this->is_active, NPC_WORKING) == NPC_SLEEP) 
		{
			local_timer->push_timer_task(this->id, TTT_MOVE_NPC, 1000, 0);
		}
	}
}

void MMONpc::update_entering_actor(MMOActor* other)
{
	AcquireSRWLockExclusive(&this->lock);
	this->view_list.emplace(other->get_id(), other);
	ReleaseSRWLockExclusive(&this->lock);
}

void MMONpc::update_leaving_actor(MMOActor* actor)
{
	AcquireSRWLockExclusive(&this->lock);
	this->view_list.erase(actor->get_id());
	ReleaseSRWLockExclusive(&this->lock);
}
