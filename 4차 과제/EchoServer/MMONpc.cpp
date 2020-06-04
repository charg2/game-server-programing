
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
	// 목표지점.. 
	// 길찾기 한 칸.
	char direction = fast_rand() % 4;
	bool is_isolated = true;
	int local_y = y;
	int local_x = x;
	int local_actor_id = id;


	is_active = NPC_SLEEP;  // 타 스레드에서 접근해서 일을 가로 챌 수 있도록...

	// 장애물 체크 등등.
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



	MMOSector* new_sector = zone->get_sector(local_y, local_x);			// view_list 긁어오기.
	// 섹터가 변경되면 바꾸기.
	if (current_sector != new_sector)
	{
		AcquireSRWLockExclusive(&current_sector->lock); // 이전 섹터 나가기
		current_sector->npcs.erase(id);
		ReleaseSRWLockExclusive(&current_sector->lock);

		// 지금 섹터 들가기.
		AcquireSRWLockExclusive(&new_sector->lock);
		this->current_sector = new_sector;
		new_sector->npcs.insert(id);
		ReleaseSRWLockExclusive(&new_sector->lock);
	}


	// 뷰리스트 수정.
	AcquireSRWLockExclusive(&lock); // // 락 걸고...
	std::map<int32_t, MMOActor*> local_old_view_list = view_list;
	ReleaseSRWLockExclusive(&lock); // // 락 걸고...


	// 주변에 뿌리기.
	MMOSector*		current_sector	= new_sector;
	const MMONear*	nears			= current_sector->get_near(local_y, local_x);
	int				near_cnt		= nears->count;

	
	// 내 주변 정보를 긁어 모음.
	std::map<int32_t, MMOActor*>	local_new_view_list;
	for (int n = 0; n < near_cnt; ++n)
	{
		AcquireSRWLockShared(&nears->sectors[n]->lock); // sector에 읽기 위해서 락을 얻고 
		for (auto& actor_iter : nears->sectors[n]->actors)
		{
			//if (actor_iter.second->status != ST_ACTIVE) 		//continue;
			if (actor_iter.second->is_near(this) == true) // 내 근처가 맞다면 넣음.
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
		if (0 == local_old_view_list.count(new_actor.first))	// 이동후 새로 보이는 유저.
		{
			this->update_entering_actor(new_actor.second);		// 새로 보이는 유저 정보를 NPC한테 업데이트 함.

			AcquireSRWLockShared(&new_actor.second->lock);
			if (0 == new_actor.second->view_list_for_npc.count(this->id)) // 타 스레드에서 시야 처리 안 된경우.
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_enter_packet(this);
			}
			else // 처리 된경우
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_move_packet(this);   // 상대 시야 리스트에 내가 있는 경우 뷰리스트만 업데이트 한다.
			}
		}
		else  // 기존 뷰리스트에 있던 유저들 
		{
			AcquireSRWLockShared(&new_actor.second->lock);					//
			if (0 != new_actor.second->view_list_for_npc.count(this->id))	// 
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_move_packet(this);
			}
			else	// 이미 나간 경우.
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_enter_packet(this);
			}
		}
	}


	//시야에서 벗어난 플레이어
	for (auto& old_it : local_old_view_list)
	{
		if (0 == local_new_view_list.count(old_it.first))
		{
			this->update_leaving_actor(old_it.second); // npc 시야에서 유저가 나간것을 업뎃 해줌.

			AcquireSRWLockShared(&old_it.second->lock);
			if (0 != old_it.second->view_list_for_npc.count(this->id))
			{
				ReleaseSRWLockShared(&old_it.second->lock);
				old_it.second->send_leave_packet(this);
			}
			else	// 이미 다른 스레드에서 지워준 경우.
			{
				ReleaseSRWLockShared(&old_it.second->lock);
			}
		}
	}


	// 주변에 아무도 없다면?
	if (is_isolated == true)
	{
		// 딴 사람이 건들였으면 딴족에서 처리해줌.
		return;
	}
	else // 있다면 업데이트 계속.
	{
		// is_active 상태의 빈 공간이 있어서 업데이트 못했을 수도 있으니 내가 해줌.
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
