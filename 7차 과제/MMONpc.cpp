
#include "pre_compile.h"
#include "MMOServer.h"
#include "mmo_function.hpp"
#include "MMONPC.h"
#include "MMOActor.h"
#include "MMONpcManager.h"
#include "MMOZone.h"
#include "MMONear.h"
#include "PathFinder.h"
#include "MMOSystmeMessage.h"


#include "script_api.h"

using namespace c2::constant;


void MMONPC::initialize_vm_and_load_data()
{
	lua_State* lua_vm = luaL_newstate();
	luaL_openlibs(lua_vm);

	int error = luaL_loadfile(lua_vm, "scripts\\npc.lua");
	if (error != 0)
	{
		printf("1lua error %s \n", lua_tostring(lua_vm, -1));
	}
	error = lua_pcall(lua_vm, 0, 0, 0);
	if (error != 0)
	{
		printf("2lua error %s \n", lua_tostring(lua_vm, -1));
	}

	// 
	lua_getglobal(lua_vm, "prepare_npc_script");
	lua_pushnumber(lua_vm, (int32_t)id);
	lua_pcall(lua_vm, 1, 0, 0);

	lua_getglobal(lua_vm, "load_mob_data");
	lua_pushnumber(lua_vm, this->type);		// 함수의 인자로 넣고 
	error = lua_pcall(lua_vm, 1, 6, 0);		// 1 파라미터, 6리턴;
	if (error != 0)
	{
		printf("load_mob_data() lua error %s \n", lua_tostring(lua_vm, -1));
	}

	// return 값
	this->x = this->initial_x =	lua_tonumber(lua_vm, -6);
	this->y = this->initial_y = lua_tonumber(lua_vm, -5);
	this->hp =		lua_tonumber(lua_vm, -4);
	this->dmg =		lua_tonumber(lua_vm, -3);
	this->level =	lua_tonumber(lua_vm, -2);
	this->exp =		lua_tonumber(lua_vm, -1);


	lua_pop(lua_vm, 6); // 다 비워냄.
	// 현상황에서는 더이상 lua를 안씀.
	//this->lua_vm = lua_vm;
	lua_close(lua_vm);
}

//void MMONPC::move()
//{
//	bool is_isolated = true;
//	int local_y = y;
//	int local_x = x;
//	int local_actor_id = id;
//
//	// 장애물 체크 등등.
//	switch (fast_rand() % 4)
//	{
//	case D_DOWN:
//		if (local_y < MAP_HEIGHT - 1)	local_y++;
//		break;
//	case D_LEFT:
//		if (local_x > 0) local_x--;
//		break;
//	case D_RIGHT:
//		if (local_x < MAP_WIDTH - 1) local_x++;
//		break;
//	case D_UP:
//		if (local_y > 0) local_y--;
//		break;
//	default:
//		size_t* invalid_ptr{}; *invalid_ptr = 0;
//		break;
//	}
//
//	x = local_x;
//	y = local_y;
//
//
//
//	MMOSector* new_sector = zone->get_sector(local_y, local_x);			// view_list 긁어오기.
//	// 섹터가 변경되면 바꾸기.
//	if (current_sector != new_sector)
//	{
//		AcquireSRWLockExclusive(&current_sector->lock); // 이전 섹터 나가기
//		current_sector->npcs.erase(id);
//		ReleaseSRWLockExclusive(&current_sector->lock);
//
//		// 지금 섹터 들가기.
//		AcquireSRWLockExclusive(&new_sector->lock);
//		this->current_sector = new_sector;
//		new_sector->npcs.insert(id);
//		ReleaseSRWLockExclusive(&new_sector->lock);
//	}
//
//
//	// 뷰리스트 수정.
//	AcquireSRWLockExclusive(&lock); // // 락 걸고...
//	std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list;
//	ReleaseSRWLockExclusive(&lock); // // 락 걸고...
//
//
//	// 주변에 뿌리기.
//	MMOSector*		current_sector	= new_sector;
//	const MMONear*	nears			= current_sector->get_near(local_y, local_x);
//	int				near_cnt		= nears->count;
//
//	
//	// 내 주변 정보를 긁어 모음.
//	std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
//	for (int n = 0; n < near_cnt; ++n)
//	{
//		AcquireSRWLockShared(&nears->sectors[n]->lock); // sector에 읽기 위해서 락을 얻고 
//		for (auto& actor_iter : nears->sectors[n]->actors)
//		{
//			//if (actor_iter.second->status != ST_ACTIVE) 		//continue;
//			if (actor_iter.second->is_near(this) == true) // 내 근처가 맞다면 넣음.
//			{
//				is_isolated = false;
//				local_new_view_list.insert(actor_iter);
//			}
//		}
//		ReleaseSRWLockShared(&nears->sectors[n]->lock);
//	}
//
//	for (auto& new_actor : local_new_view_list)
//	{
//		if (0 == local_old_view_list.count(new_actor.first))	// 이동후 새로 보이는 유저.
//		{
//			this->update_entering_actor(new_actor.second);		// 새로 보이는 유저 정보를 NPC한테 업데이트 함.
//
//			AcquireSRWLockShared(&new_actor.second->lock);
//			if (0 == new_actor.second->view_list_for_npc.count(this->id)) // 타 스레드에서 시야 처리 안 된경우.
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_enter_packet(this);
//			}
//			else // 처리 된경우
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_move_packet(this);   // 상대 시야 리스트에 내가 있는 경우 뷰리스트만 업데이트 한다.
//			}
//		}
//		else  // 기존 뷰리스트에 있던 유저들 
//		{
//			AcquireSRWLockShared(&new_actor.second->lock);					//
//			if (0 != new_actor.second->view_list_for_npc.count(this->id))	// 
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_move_packet(this);
//			}
//			else	// 이미 나간 경우.
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_enter_packet(this);
//			}
//		}
//	}
//
//
//	//시야에서 벗어난 플레이어
//	for (auto& old_it : local_old_view_list)
//	{
//		if (0 == local_new_view_list.count(old_it.first))		// 현재 내 시야에 없는 플레이어.
//		{														// 
//			this->update_leaving_actor(old_it.second);			// npc 시야에서 유저가 나간것을 업뎃 해줌.
//			
//			AcquireSRWLockShared(&old_it.second->lock);			// 
//			if (0 != old_it.second->view_list_for_npc.count(this->id)) // 클라 시야에 내가 있는 경우
//			{
//				ReleaseSRWLockShared(&old_it.second->lock);
//				old_it.second->send_leave_packet(this);
//			}
//			else	// 이미 다른 스레드에서 지워준 경우.
//			{
//				ReleaseSRWLockShared(&old_it.second->lock);
//			}
//		}
//	}
//
//	// 주변에 아무도 없다면?
//	if (is_isolated == true)
//	{
//		// 딴 사람이 건들였으면 딴족에서 처리해줌.
//		// 타 스레드에서 접근해서 일을 가로 챌 수 있도록...
//		is_active = NPC_SLEEP;
//	}
//	else // 있다면 업데이트 계속.
//	{
//		// is_active 상태의 빈 공간이 있어서 업데이트 못했을 수도 있으니 내가 해줌.
//		local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
//	}
//}

void MMONPC::move_to_anywhere()
{
	if (false == this->is_alive)
	{
		return; 
	}

	bool is_isolated = true;
	int local_y = y;
	int local_x = x;
	int local_actor_id = id;

	switch (fast_rand() % 4)
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

	MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list 긁어오기.
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
	AcquireSRWLockShared(&lock); // read 락 걸고...
	std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
	ReleaseSRWLockShared(&lock);


	// 여기 이후로 사망하면... 

	// 주변에 뿌리기.
	MMOSector*		current_sector	= new_sector;
	const MMONear*	nears			= current_sector->get_near(local_y, local_x);
	int				near_cnt		= nears->count;


	// 내 주변 정보를 긁어 모음.
	std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
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
		if (0 == local_new_view_list.count(old_it.first))		// 현재 내 시야에 없는 플레이어.
		{														// 
			this->update_leaving_actor(old_it.second);			// npc 시야에서 유저가 나간것을 업뎃 해줌.

			AcquireSRWLockShared(&old_it.second->lock);			// 
			if (0 != old_it.second->view_list_for_npc.count(this->id)) // 클라 시야에 내가 있는 경우
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

	if (is_isolated == true)
	{
		is_active = NPC_SLEEP;
		// 최후 확인용 작업 // 이작업중에 누가 
		//local_timer->push_timer_task(this->id, TTT_ON_SLEEP, 1000, 0);
	}
	else // 있다면 업데이트 계속.
	{
		local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
	}
}

void MMONPC::initialize(size_t id_base)
{
	current_sector = nullptr;
	
	id = id_base + c2::constant::NPC_ID_OFFSET;
	max_hp = 200;

	is_alive = true;

	const wchar_t* npc_tag = L"NPC";
	memcpy(this->name, npc_tag, 8);

	target = nullptr;
	//hp = 200;
	//x = rand() % c2::constant::MAP_WIDTH;
	//y = rand() % c2::constant::MAP_HEIGHT;
	zone = g_zone;
	is_active = NPC_SLEEP;
}

void MMONPC::reset()
{
	current_sector = nullptr;
	target = nullptr;
	has_target = false;


	hp = 200;
	max_hp = 200;

	target = nullptr;

	this->view_list.clear();

	//x = rand() % c2::constant::MAP_WIDTH;
	//y = rand() % c2::constant::MAP_HEIGHT;

	zone = this->zone;
	is_alive = true;
	is_active = NPC_SLEEP;
}

void MMONPC::respawn() // 근데 이때는 아무도 모르는 상태이기 때문에 락을 안걸고 해도 되긴하는데...
{
	zone->enter_npc(this);

	// 주변 시야 얻고.
	MMOSector*		current_sector = zone->get_sector(y , x);
	const MMONear*	nears		= g_zone->get_near(this->y, this->x);
	int				near_cnt	= nears->count;

	std::unordered_map<int32_t, MMOActor*>	local_view_list;
	for (size_t n{ }; n < nears->count; ++n)
	{
		MMOSector* near_sector = nears->sectors[n];
		AcquireSRWLockShared(&near_sector->lock); //sector에 읽기 위해서 락을 얻고 
		
		for (auto& other_iter : near_sector->actors)
		{
			if (other_iter.second->is_near(this) == true) // 근처가 맞다면 넣음.
				local_view_list.insert(other_iter);
		}

		//NPC 처리 로직.
		ReleaseSRWLockShared(&near_sector->lock);
	}


	// 유저들에게 접소을 알리고 내 뷰리스트에 추가함.
	for (auto& iter : local_view_list)
	{
		MMOActor* other = iter.second;
		
		other->send_enter_packet(this); // NPC 정보를 상대방한테 보내고 뷰리스트추가..
		// 내 뷰리스트 추가는 뒤로 미룸.
	}

	// 내 뷰리스트 업뎃
	AcquireSRWLockExclusive(&this->lock);							//내 view_list 에 접근하기 읽기 위해서 락을 얻고 
	this->view_list = std::move(local_view_list);
	ReleaseSRWLockExclusive(&this->lock);
}

void MMONPC::attack_for_peace()
{
	bool local_has_target = has_target;

	if (false == local_has_target || false == is_in_attack_range(target)) // 대상이 없거나... 범위에서 사라졌다면 종료.
	{
		//is_active = NPC_SLEEP;
		// 공격 1회 후 공격을 않마.
		return;
	}

	int ys[5];
	int xs[5];

	int effective_position_count{};

	int y = this->y;
	int x = this->x;

	//std::unordered_map<int32_t, MMOActor*> user_attack_list;

	// 주변 4방향 좌표를 구하고 검사를 함. 장애물이 있는 곳 or 인덱스 범위를 초과 하는 곳이면 그곳은 제외한다. (  user만 팬다. ) 
	if (y - 1 >= 0 && g_zone->has_obstacle(y - 1, x) == false)
	{
		ys[effective_position_count] = y - 1; xs[effective_position_count] = x;
		effective_position_count += 1;
	}
	if (y + 1 < c2::constant::MAP_HEIGHT && g_zone->has_obstacle(y + 1, x) == false)
	{
		ys[effective_position_count] = y + 1; xs[effective_position_count] = x;
		effective_position_count += 1;
	}
	if (x - 1 >= 0 && g_zone->has_obstacle(y, x - 1) == false)
	{
		ys[effective_position_count] = y; xs[effective_position_count] = x - 1;
		effective_position_count += 1;
	}
	if (x + 1 < c2::constant::MAP_WIDTH && g_zone->has_obstacle(y, x + 1) == false)
	{
		ys[effective_position_count] = y; xs[effective_position_count] = x + 1;
		effective_position_count += 1;
	}

	ys[effective_position_count] = y;
	xs[effective_position_count] = x;
	effective_position_count += 1;

	AcquireSRWLockShared(&lock); // 섹터에 npc에 대한 읽기 작업만.
	auto& local_view_list = view_list;
	ReleaseSRWLockShared(&lock);

	for (auto& user_it : local_view_list)
	{
		MMOActor* user = user_it.second;   // 

		for (int k = 0; k < effective_position_count; ++k)
		{
			if (ys[k] == user->y && xs[k] == user->x)	// 좌표가 일치하면 
			{
				if (user->is_alive == true)
				{
					user->decrease_hp(this, this->dmg);
					send_chat_packet_to_target(user, system_msg_log_attack);
				}
			}
		}
	}
}

void MMONPC::die()
{
}
	
void MMONPC::send_chatting_to_actor(int32_t actor_id ,wchar_t* message)
{
	sc_packet_chat chat_payload;
	chat_payload.header.length = sizeof(sc_packet_chat);
	chat_payload.header.type = S2C_CHAT;
	chat_payload.id = this->id;
	wcscpy_s(chat_payload.chat, message);
	
	c2::Packet* chat_packet = c2::Packet::alloc();
	chat_packet->write(&chat_payload, sizeof(sc_packet_chat));

	g_zone->server->send_packet(actor_id, chat_packet);
}

void MMONPC::update_for_fixed_peace()
{
	if (false == this->is_alive) // 내가 죽은 상태면 종료.
	{
		is_active = NPC_SLEEP;
		has_target = false;

		return;
	}

	MMOActor* local_target		= this->target; 
	bool	  local_has_target	= this->has_target;
	if (false == has_target || false == is_in_attack_range(local_target)) // 대상이 없거나 거리가 멀어지면 종료.
	{
		is_active = NPC_SLEEP;
		has_target = false;

		return;
	}

	if (nullptr != local_target && true == local_target->is_alive)			// 타겟이 있을시에만 날 찾아옴.
	{
		bool is_isolated = true;
		int local_actor_id = id;

		int temp_local_y;
		int temp_local_x;
		int local_y = temp_local_y = y;
		int local_x = temp_local_x = x;

		int temp_local_target_y;
		int temp_local_target_x;
		int local_target_y = temp_local_target_y = target->y;
		int local_target_x = temp_local_target_x = target->x;



		bool has_obstacle = true;
		for (;;) // 직선 경로에 장애물이 없다면 그냥   		// 최단 경로 찾기.
		{
			if (temp_local_x > temp_local_target_x) // x 좌표
			{
				temp_local_x -= 1;
			}
			else if (temp_local_x < temp_local_target_x)
			{
				temp_local_x += 1;
			}

			if (temp_local_y > temp_local_target_y) // y 좌표
			{
				temp_local_y -= 1;
			}
			else if (temp_local_y < temp_local_target_y)
			{
				temp_local_y += 1;
			}

			if (c2::global::obstacle_table[(temp_local_y * c2::constant::MAP_HEIGHT) + temp_local_x]) // 중간에 장애물이 있다면?
			{
				break; // 종료 -> astr로 길찾기.
			}

			if (temp_local_x == temp_local_target_x && temp_local_y == temp_local_target_y) // 가는 동안 장앵물이 없었따면?
			{
				if (local_x > local_target_x) // x 좌표
				{
					local_x -= 1;
				}
				else if (local_x < local_target_x)
				{
					local_x += 1;
				}

				if (local_y > local_target_y) // y 좌표
				{
					local_y -= 1;
				}
				else if (local_y < local_target_y)
				{
					local_y += 1;
				}

				has_obstacle = false;

				if (local_x != local_target_x || local_y != local_target_y)
				{
					x = local_x;
					y = local_y;

					break;
				}
				else
				{
					local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
					return;
				}
			}
		}

		if (true == has_obstacle)  // 장애물이 있다면 astar로 길 찾기.
		{
			printf("astar");

			if (true == PathFindingHelper->NewPath(local_x, local_y, local_target_x, local_target_y))
			{
				if (true == PathFindingHelper->PathNextNode())
				{
					x = local_x = PathFindingHelper->NodeGetX();
					y = local_y = PathFindingHelper->NodeGetY();
				}
			}
		}

	


		MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list 긁어오기.
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
		AcquireSRWLockShared(&lock); // read 락 걸고...
		std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
		ReleaseSRWLockShared(&lock);


		// 주변에 뿌리기.
		MMOSector*		current_sector	= new_sector;
		const MMONear*	nears			= current_sector->get_near(local_y, local_x);
		int				near_cnt		= nears->count;

		// 내 주변 정보를 긁어 모음.
		std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
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

			if (true == is_in_attack_range(new_actor.second))
			{
				set_target(new_actor.second);
			}
		}


		//시야에서 벗어난 플레이어
		for (auto& old_it : local_old_view_list)
		{
			if (0 == local_new_view_list.count(old_it.first))		// 현재 내 시야에 없는 플레이어.
			{														// 
				this->update_leaving_actor(old_it.second);			// npc 시야에서 유저가 나간것을 업뎃 해줌.

				AcquireSRWLockShared(&old_it.second->lock);			// 
				if (0 != old_it.second->view_list_for_npc.count(this->id)) // 클라 시야에 내가 있는 경우
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

		if (is_isolated == true)
		{
			is_active = NPC_SLEEP;
			//target = nullptr;
			has_target = false;
			// 최후 확인용 작업 // 이작업중에 누가 
		}
		else // 있다면 업데이트 계속.
		{
			local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
		}
	}
	else
	{
		is_active = NPC_SLEEP;
		//target = nullptr;
		has_target = false;
	}
}

void MMONPC::update_for_peace()
{
	if (false == this->is_alive) // 내가 죽은 상태면 종료.
	{
		is_active = NPC_SLEEP;
		has_target = false;

		return;
	}

	MMOActor* local_target = this->target;
	bool	  local_has_target = this->has_target;

	if ( false == local_has_target || false == is_in_attack_range(target) ) // 대상이 없거나 거리가 멀어지면 종료.
	{
		is_active = NPC_SLEEP;

		return;
	}


	if (nullptr != local_target && true == local_target->is_alive)			// 타겟이 있을시에만 날 찾아옴.
	{
		bool is_isolated = true;
		int local_actor_id = id;

		int temp_local_y;
		int temp_local_x;
		int local_y = temp_local_y = y;
		int local_x = temp_local_x = x;
		
		int temp_local_target_y;
		int temp_local_target_x;
		int local_target_y = temp_local_target_y = target->y;
		int local_target_x = temp_local_target_x = target->x;

		

		bool has_obstacle = true;
		for (;;) // 직선 경로에 장애물이 없다면 그냥   		// 최단 경로 찾기.
		{
			if (temp_local_x > temp_local_target_x) // x 좌표
			{
				temp_local_x -= 1;
			}
			else if (temp_local_x < temp_local_target_x)
			{
				temp_local_x += 1;
			}

			if (temp_local_y > temp_local_target_y) // y 좌표
			{
				temp_local_y -= 1;
			}
			else if (temp_local_y < temp_local_target_y)
			{
				temp_local_y += 1;
			}

			if (c2::global::obstacle_table[(temp_local_y * c2::constant::MAP_HEIGHT) + temp_local_x]) // 중간에 장애물이 있다면?
			//if (c2::global::obstacle_table[(temp_local_x * c2::constant::MAP_WIDTH) + temp_local_y]) // 중간에 장애물이 있다면?
			{
				break; // 종료 -> astr로 길찾기.
			}

			if (temp_local_x == temp_local_target_x && temp_local_y == temp_local_target_y ) // 가는 동안 장앵물이 없었따면?
			{
				if (local_x > local_target_x) // x 좌표
				{
					local_x -= 1;
				}
				else if (local_x < local_target_x)
				{
					local_x += 1;
				}

				if (local_y > local_target_y) // y 좌표
				{
					local_y -= 1;
				}
				else if (local_y < local_target_y)
				{
					local_y += 1;
				}

				has_obstacle = false;

				if (local_x != local_target_x || local_y != local_target_y)
				{
					x = local_x;
					y = local_y;

					break;
				}
				else
				{
					local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
					return;
				}
			}
		}



		if(true == has_obstacle)  // 장애물이 있다면 astar로 길 찾기.
		{
			printf("astars\n");

			PathFindingHelper->NewPath(local_x, local_y, local_target->x, local_target->y);

			x = local_x = PathFindingHelper->NodeGetX();
			y = local_y = PathFindingHelper->NodeGetY();
		}



		MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list 긁어오기.
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
		AcquireSRWLockShared(&lock); // read 락 걸고...
		std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
		ReleaseSRWLockShared(&lock);



		// 주변에 뿌리기.
		MMOSector*		current_sector	= new_sector;
		const MMONear*	nears			= current_sector->get_near(local_y, local_x);
		int				near_cnt		= nears->count;



		// 내 주변 정보를 긁어 모음.
		std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
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

			if (true == is_in_attack_range(new_actor.second))
			{
				set_target(new_actor.second);
			}
		}


		//시야에서 벗어난 플레이어
		for (auto& old_it : local_old_view_list)
		{
			if (0 == local_new_view_list.count(old_it.first))		// 현재 내 시야에 없는 플레이어.
			{														// 
				this->update_leaving_actor(old_it.second);			// npc 시야에서 유저가 나간것을 업뎃 해줌.

				AcquireSRWLockShared(&old_it.second->lock);			// 
				if (0 != old_it.second->view_list_for_npc.count(this->id)) // 클라 시야에 내가 있는 경우
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

		if (is_isolated == true)
		{
			is_active = NPC_SLEEP;
			//target = nullptr;
			has_target = false;
			// 최후 확인용 작업 // 이작업중에 누가 
		}
		else // 있다면 업데이트 계속.
		{
			local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
		}
	}
	else
	{
		is_active = NPC_SLEEP;
		//target = nullptr;
		has_target = false;
	}
}

void MMONPC::update_for_fixed_combat()
{
	if (false == this->is_alive)
	{
		return;
	}

	MMOActor* local_target = this->target;
	bool	  local_has_target = this->has_target;
	if (false == has_target || false == is_in_attack_range(local_target)) // 대상이 없거나 거리가 멀어지면 종료.
	{
		is_active = NPC_SLEEP;
		has_target = false;

		return;
	}

	if (nullptr != local_target && true == local_target->is_alive)			// 타겟이 있을시에만 날 찾아옴.
	{
		bool is_isolated = true;
		int local_actor_id = id;

		int temp_local_y;
		int temp_local_x;
		int local_y = temp_local_y = y;
		int local_x = temp_local_x = x;

		int temp_local_target_y;
		int temp_local_target_x;
		int local_target_y = temp_local_target_y = target->y;
		int local_target_x = temp_local_target_x = target->x;



		bool has_obstacle = true;
		for (;;) // 직선 경로에 장애물이 없다면 그냥   		// 최단 경로 찾기.
		{
			if (temp_local_x > temp_local_target_x) // x 좌표
			{
				temp_local_x -= 1;
			}
			else if (temp_local_x < temp_local_target_x)
			{
				temp_local_x += 1;
			}

			if (temp_local_y > temp_local_target_y) // y 좌표
			{
				temp_local_y -= 1;
			}
			else if (temp_local_y < temp_local_target_y)
			{
				temp_local_y += 1;
			}

			if (c2::global::obstacle_table[(temp_local_y * c2::constant::MAP_HEIGHT) + temp_local_x]) // 중간에 장애물이 있다면?
			{
				break; // 종료 -> astr로 길찾기.
			}

			if (temp_local_x == temp_local_target_x && temp_local_y == temp_local_target_y) // 가는 동안 장앵물이 없었따면?
			{
				if (local_x > local_target_x) // x 좌표
				{
					local_x -= 1;
				}
				else if (local_x < local_target_x)
				{
					local_x += 1;
				}

				if (local_y > local_target_y) // y 좌표
				{
					local_y -= 1;
				}
				else if (local_y < local_target_y)
				{
					local_y += 1;
				}

				has_obstacle = false;

				if (local_x != local_target_x || local_y != local_target_y)
				{
					x = local_x;
					y = local_y;

					break;
				}
				else
				{
					local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
					return;
				}
			}
		}

		if (true == has_obstacle)  // 장애물이 있다면 astar로 길 찾기.
		{
			if (true == PathFindingHelper->NewPath(local_x, local_y, local_target_x, local_target_y))
			{
				if (true == PathFindingHelper->PathNextNode())
				{
					x = local_x = PathFindingHelper->NodeGetX();
					y = local_y = PathFindingHelper->NodeGetY();
				}
			}
		}

		MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list 긁어오기.
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
		AcquireSRWLockShared(&lock); // read 락 걸고...
		std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
		ReleaseSRWLockShared(&lock);


		// 여기 이후로 사망하면... 

		// 주변에 뿌리기.
		MMOSector* current_sector = new_sector;
		const MMONear* nears = current_sector->get_near(local_y, local_x);
		int				near_cnt = nears->count;


		// 내 주변 정보를 긁어 모음.
		std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
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

			if (true == is_in_attack_range(new_actor.second))
			{
				set_target(new_actor.second);
			}
		}


		//시야에서 벗어난 플레이어
		for (auto& old_it : local_old_view_list)
		{
			if (0 == local_new_view_list.count(old_it.first))		// 현재 내 시야에 없는 플레이어.
			{														// 
				this->update_leaving_actor(old_it.second);			// npc 시야에서 유저가 나간것을 업뎃 해줌.

				AcquireSRWLockShared(&old_it.second->lock);			// 
				if (0 != old_it.second->view_list_for_npc.count(this->id)) // 클라 시야에 내가 있는 경우
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

		if (is_isolated == true)
		{
			is_active = NPC_SLEEP;
			//target = nullptr;
			has_target = false;
			// 최후 확인용 작업 // 이작업중에 누가 
			//local_timer->push_timer_task(this->id, TTT_ON_SLEEP, 1000, 0);
		}
		else // 있다면 업데이트 계속.
		{
			local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
		}
	}
	else
	{
		is_active	= NPC_SLEEP;
		//target	= nullptr;
		has_target	= false;
	}
}

void MMONPC::update_for_combat()
{
	if (false == this->is_alive)
	{
		return;
	}

	bool is_isolated = true;
	int local_y = y;
	int local_x = x;
	int local_actor_id = id;

	MMOActor* local_target = this->target;
	

	if (nullptr != local_target && true == local_target->is_alive)			// 타겟이 있을시에만 날 찾아옴.
	{
		bool is_isolated = true;
		int local_actor_id = id;

		int temp_local_y;
		int temp_local_x;
		int local_y = temp_local_y = y;
		int local_x = temp_local_x = x;

		int temp_local_target_y;
		int temp_local_target_x;
		int local_target_y = temp_local_target_y = target->y;
		int local_target_x = temp_local_target_x = target->x;



		bool has_obstacle = true;
		for (;;) // 직선 경로에 장애물이 없다면 그냥   		// 최단 경로 찾기.
		{
			if (temp_local_x > temp_local_target_x) // x 좌표
			{
				temp_local_x -= 1;
			}
			else if (temp_local_x < temp_local_target_x)
			{
				temp_local_x += 1;
			}

			if (temp_local_y > temp_local_target_y) // y 좌표
			{
				temp_local_y -= 1;
			}
			else if (temp_local_y < temp_local_target_y)
			{
				temp_local_y += 1;
			}

			if (c2::global::obstacle_table[(temp_local_y * c2::constant::MAP_HEIGHT) + temp_local_x]) // 중간에 장애물이 있다면?
			{
				break; // 종료 -> astr로 길찾기.
			}

			if (temp_local_x == temp_local_target_x && temp_local_y == temp_local_target_y) // 가는 동안 장앵물이 없었따면?
			{
				if (local_x > local_target_x) // x 좌표
				{
					local_x -= 1;
				}
				else if (local_x < local_target_x)
				{
					local_x += 1;
				}

				if (local_y > local_target_y) // y 좌표
				{
					local_y -= 1;
				}
				else if (local_y < local_target_y)
				{
					local_y += 1;
				}

				has_obstacle = false;

				if (local_x != local_target_x || local_y != local_target_y)
				{
					x = local_x;
					y = local_y;

					break;
				}
				else
				{
					local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
					return;
				}
			}
		}

		if (true == has_obstacle)  // 장애물이 있다면 astar로 길 찾기.
		{
			if (true == PathFindingHelper->NewPath(local_x, local_y, local_target_x, local_target_y))
			{
				if (true == PathFindingHelper->PathNextNode())
				{
					x = local_x = PathFindingHelper->NodeGetX();
					y = local_y = PathFindingHelper->NodeGetY();
				}
			}
		}

		MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list 긁어오기.
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
		AcquireSRWLockShared(&lock); // read 락 걸고...
		std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
		ReleaseSRWLockShared(&lock);


		// 여기 이후로 사망하면... 

		// 주변에 뿌리기.
		MMOSector* current_sector = new_sector;
		const MMONear* nears = current_sector->get_near(local_y, local_x);
		int				near_cnt = nears->count;


		// 내 주변 정보를 긁어 모음.
		std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
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

		for (auto& new_actor : local_new_view_list)
		{
			if (0 == local_old_view_list.count(new_actor.first))	// 이동후 새로 보이는 유저.
			{
				this->update_entering_actor(new_actor.second);		

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

			if (true == is_in_attack_range(new_actor.second))
			{
				set_target(new_actor.second);
			}
		}


		//시야에서 벗어난 플레이어
		for (auto& old_it : local_old_view_list)
		{
			if (0 == local_new_view_list.count(old_it.first))		// 현재 내 시야에 없는 플레이어.
			{														// 
				this->update_leaving_actor(old_it.second);			// npc 시야에서 유저가 나간것을 업뎃 해줌.

				AcquireSRWLockShared(&old_it.second->lock);			// 
				if (0 != old_it.second->view_list_for_npc.count(this->id)) // 클라 시야에 내가 있는 경우
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

		if (is_isolated == true)
		{
			is_active = NPC_SLEEP;
			// 최후 확인용 작업 // 이작업중에 누가 
			//local_timer->push_timer_task(this->id, TTT_ON_SLEEP, 1000, 0);
		}
		else // 있다면 업데이트 계속.
		{
			local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
		}
	}
	else
	{
		is_active = NPC_SLEEP;
		has_target = false;
	}
}

void MMONPC::set_target(MMOActor* actor)
{
	if (false == has_target && false == InterlockedExchange8((volatile char*)&has_target, true))
	{
		printf("%d target : %d \n", this->type, actor->session_id);
		this->target = actor;
	}
}


void MMONPC::decrease_hp(MMOActor* actor, int32_t damage)
{
	AcquireSRWLockExclusive(&this->lock);
	// 공격 하고 NPC 죽었으면 죽었다고 상태 바꾸기 ㅇㅇ;

	
	hp -= damage;
	if (hp <= 0)
	{
		if ( this->is_alive != false && true == InterlockedExchange8((volatile CHAR*)&is_alive, 0))// = false; // 공격 하고 NPC 죽었으면 죽었다고 상태 바꾸기 ㅇㅇ
		{
			AcquireSRWLockExclusive(&current_sector->lock); // 현재 섹터 나가기. // 다른 클라접근 하기 힘들게..
			current_sector->npcs.erase(id);
			ReleaseSRWLockExclusive(&current_sector->lock);

			actor->increase_exp(exp);// 죽인거면 경험치를 얻고 그에 대한 처리를 한다.

			for (auto& iter : view_list)	// npc가 주변에 브로드 캐스팅함. 죽어서 나갔다고 
			{
				MMOActor* neighbor = iter.second;

				neighbor->send_leave_packet(this); // 내 시야리스트 플레이어게 나간다고 알림.
			}

			ReleaseSRWLockExclusive(&this->lock); // 여기선 락을 푼다.
			
			send_chat_packet_to_target(actor, system_msg_hit);

			local_timer->push_timer_task(id, TTT_RESPAWN_FOR_NPC, 30'000, 0);
		}
		else
		{
			ReleaseSRWLockExclusive(&this->lock);

			send_chat_packet_to_target(actor, system_msg_hit);
			return;
		}
	}
	else
	{
		send_chat_packet_to_target(actor, system_msg_hit);
		// 마지막으로 타이머에 30초후 리스폰 이벤트를 추가.
		//else  // 현재 프로토콜상 체력 깍이는건 안알랴줘도 된다.//{//}
		ReleaseSRWLockExclusive(&this->lock); // 여기선 락을 푼다.
		return;
	}
}

bool MMONPC::is_in_attack_range(MMOActor* actor)
{
	if (abs(this->x - actor->x) > FOV_HALF_WIDTH_FOR_COMBAT_MOB) return false;
	if (abs(this->y - actor->y) > FOV_HALF_HEIGHT_FOR_COMBAT_MOB) return false;

	return true;
}

void MMONPC::send_chat_packet(const wchar_t* msg)
{
	AcquireSRWLockShared(&lock); // 섹터에 npc에 대한 읽기 작업만.
	std::unordered_map<int32_t, MMOActor*> local_view_list = view_list;		// 임시 뷰리스트.
	ReleaseSRWLockShared(&lock);

	sc_packet_chat chat_payload; 	// id check
	chat_payload.header.type = S2C_CHAT;
	chat_payload.header.length = sizeof(sc_packet_chat);
	wcsncpy_s(chat_payload.chat, msg, c2::constant::MAX_CHAT_LEN);


	c2::Packet* out_packet = c2::Packet::alloc();
	out_packet->write(&chat_payload, sizeof(sc_packet_chat));
	out_packet->add_ref(local_view_list.size()); 

	for (auto& actor_it : local_view_list)
	{
		g_server->send_packet(actor_it.second->session_id, out_packet);	
	}

	out_packet->decrease_ref_count();
}

void MMONPC::send_chat_packet_to_target(MMOActor* target, const wchar_t* msg)
{
	sc_packet_chat chat_payload; 	// id check
	chat_payload.header.type = S2C_CHAT;
	chat_payload.header.length = sizeof(sc_packet_chat);
	wcsncpy_s(chat_payload.chat, msg, c2::constant::MAX_CHAT_LEN);

	c2::Packet* out_packet = c2::Packet::alloc();
	out_packet->write(&chat_payload, sizeof(sc_packet_chat));

	g_server->send_packet(target->session_id, out_packet);
}


void MMONPC::update_entering_actor(MMOActor* other)
{
	AcquireSRWLockExclusive(&this->lock);
	this->view_list.emplace(other->get_id(), other);
	ReleaseSRWLockExclusive(&this->lock);
}

void MMONPC::update_leaving_actor(MMOActor* actor)
{
	AcquireSRWLockExclusive(&this->lock);
	this->view_list.erase(actor->get_id());
	ReleaseSRWLockExclusive(&this->lock);
}
