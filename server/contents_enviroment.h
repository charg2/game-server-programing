#pragma once

#include "mmo_function.hpp"

namespace c2
{
	namespace constant
	{

		constexpr unsigned char D_UP = 0;
		constexpr unsigned char D_DOWN = 1;
		constexpr unsigned char D_LEFT = 2;
		constexpr unsigned char D_RIGHT = 3;

		constexpr unsigned char O_HUMAN = 0;
		constexpr unsigned char O_ELF = 1;
		constexpr unsigned char O_ORC = 2;

		constexpr int MAX_ID_LEN = 50;
		constexpr int MAX_STR_LEN = 80;
		constexpr int MAX_CHAT_LEN = 80;

		constexpr int FOV_WIDTH = 19;
		constexpr int FOV_HEIGHT = 19;

		constexpr int SECTOR_WIDTH = FOV_WIDTH * 2;
		constexpr int SECTOR_HEIGHT = FOV_HEIGHT * 2;

		constexpr int FOV_HALF_WIDTH = FOV_WIDTH / 2;
		constexpr int FOV_HALF_HEIGHT = FOV_HEIGHT / 2;


		constexpr int FOV_HALF_WIDTH_FOR_COMBAT_MOB = 5;
		constexpr int FOV_HALF_HEIGHT_FOR_COMBAT_MOB = 5;

		constexpr int BROADCAST_HEIGHT = 11;
		constexpr int BROADCAST_WIDTH = 11;
		constexpr int MAP_WIDTH = 800;
		constexpr int MAP_HEIGHT = 800;

		constexpr unsigned char TEST_DMG = 50;

		constexpr int NPC_ID_OFFSET = 10'000;
		constexpr size_t MAP_NAX_HEIGHT_INDEX = calc_static_map_index<MAP_HEIGHT, SECTOR_HEIGHT>();
		constexpr size_t MAP_NAX_WIDTH_INDEX = calc_static_map_index<MAP_WIDTH, SECTOR_WIDTH>();


		constexpr int INITALI_DAMAGE = 10;

		constexpr int INITIAL_HP = 200;
		constexpr int LEVEL_MAXIMUM_EXP = 200;  // level * LEVLE_MAXIMUM_EXP하면 됨.

	}


	namespace enumeration
	{
		enum MMOActorState : unsigned long long
		{
			ACS_NONE,
			ACS_BEFORE_LOGIN,
			ACS_AFTER_LOGIN,
			ACS_PLAYING,


		};
	}

	namespace global
	{
		extern inline wchar_t	db_connection_string[128]			{ };
		extern inline int32_t	db_read_thread_count				{ - 1};
		extern inline uint32_t	max_npc								{ };
		extern inline int32_t	concurrent_db_reader_thread_count	{ -1 };
		extern inline wchar_t	db_server_name[64]					{ };

		extern inline char			obstacle_table[800 * 800]			{};
		extern inline int32_t		obstacle_table_width				{};
		extern inline int32_t		obstacle_table_height				{};
	}

	namespace local
	{
		extern inline thread_local int32_t	db_thread_id	{ -1 };
	}
}


//
//
//void MMONPC::update_for_peace()
//{
//	if (false == this->is_alive) // 내가 죽은 상태면 종료.
//	{
//		is_active = NPC_SLEEP;
//		has_target = false;
//
//		return;
//	}
//
//	MMOActor* local_target = this->target;
//	bool	  local_has_target = this->has_target;
//
//	if (false == local_has_target || false == is_near(local_target)) // 대상이 없거나 거리가 멀어지면 종료.
//	{
//		is_active = NPC_SLEEP;
//
//		return;
//	}
//
//
//	if (nullptr != local_target && true == local_target->is_alive)			// 타겟이 있을시에만 날 찾아옴.
//	{
//		bool is_isolated = true;
//		int local_actor_id = id;
//
//		int temp_local_y;
//		int temp_local_x;
//		int local_y = temp_local_y = y;
//		int local_x = temp_local_x = x;
//
//		int temp_local_target_y;
//		int temp_local_target_x;
//		int local_target_y = temp_local_target_y = target->y;
//		int local_target_x = temp_local_target_x = target->x;
//
//
//
//		bool has_obstacle = true;
//		for (;;) // 직선 경로에 장애물이 없다면 그냥   		// 최단 경로 찾기.
//		{
//			if (temp_local_x > temp_local_target_x) // x 좌표
//			{
//				temp_local_x -= 1;
//			}
//			else if (temp_local_x < temp_local_target_x)
//			{
//				temp_local_x += 1;
//			}
//
//			if (temp_local_y > temp_local_target_y) // y 좌표
//			{
//				temp_local_y -= 1;
//			}
//			else if (temp_local_y < temp_local_target_y)
//			{
//				temp_local_y += 1;
//			}
//
//			if (c2::global::obstacle_table[(temp_local_y * c2::constant::MAP_HEIGHT) + temp_local_x]) // 중간에 장애물이 있다면?
//			{
//				break; // 종료 -> astr로 길찾기.
//			}
//
//			if (temp_local_x == temp_local_target_x && temp_local_y == temp_local_target_y) // 가는 동안 장앵물이 없었따면?
//			{
//				if (local_x > local_target_x) // x 좌표
//				{
//					local_x -= 1;
//				}
//				else if (local_x < local_target_x)
//				{
//					local_x += 1;
//				}
//
//				if (local_y > local_target_y) // y 좌표
//				{
//					local_y -= 1;
//				}
//				else if (local_y < local_target_y)
//				{
//					local_y += 1;
//				}
//
//				has_obstacle = false;
//
//				if (local_x != local_target_x || local_y != local_target_y)
//				{
//					x = local_x;
//					y = local_y;
//				}
//				else
//				{
//					return;
//				}
//			}
//		}
//
//
//
//		if (true == has_obstacle)  // 장애물이 있다면 astar로 길 찾기.
//		{
//			printf("astars\n");
//
//			PathFindingHelper->NewPath(local_x, local_y, local_target->x, local_target->y);
//
//			x = local_x = PathFindingHelper->NodeGetX();
//			y = local_y = PathFindingHelper->NodeGetY();
//		}
//
//
//
//		MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list 긁어오기.
//		// 섹터가 변경되면 바꾸기.
//		if (current_sector != new_sector)
//		{
//			AcquireSRWLockExclusive(&current_sector->lock); // 이전 섹터 나가기
//			current_sector->npcs.erase(id);
//			ReleaseSRWLockExclusive(&current_sector->lock);
//
//			// 지금 섹터 들가기.
//			AcquireSRWLockExclusive(&new_sector->lock);
//			this->current_sector = new_sector;
//			new_sector->npcs.insert(id);
//			ReleaseSRWLockExclusive(&new_sector->lock);
//		}
//
//
//		// 뷰리스트 수정.
//		AcquireSRWLockShared(&lock); // read 락 걸고...
//		std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
//		ReleaseSRWLockShared(&lock);
//
//
//
//		// 주변에 뿌리기.
//		MMOSector* current_sector = new_sector;
//		const MMONear* nears = current_sector->get_near(local_y, local_x);
//		int				near_cnt = nears->count;
//
//
//
//		// 내 주변 정보를 긁어 모음.
//		std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
//		for (int n = 0; n < near_cnt; ++n)
//		{
//			AcquireSRWLockShared(&nears->sectors[n]->lock); // sector에 읽기 위해서 락을 얻고 
//			for (auto& actor_iter : nears->sectors[n]->actors)
//			{
//				//if (actor_iter.second->status != ST_ACTIVE) 		//continue;
//				if (actor_iter.second->is_near(this) == true) // 내 근처가 맞다면 넣음.
//				{
//					is_isolated = false;
//					local_new_view_list.insert(actor_iter);
//				}
//			}
//			ReleaseSRWLockShared(&nears->sectors[n]->lock);
//		}
//
//		for (auto& new_actor : local_new_view_list)
//		{
//			if (0 == local_old_view_list.count(new_actor.first))	// 이동후 새로 보이는 유저.
//			{
//				this->update_entering_actor(new_actor.second);		// 새로 보이는 유저 정보를 NPC한테 업데이트 함.
//
//				AcquireSRWLockShared(&new_actor.second->lock);
//				if (0 == new_actor.second->view_list_for_npc.count(this->id)) // 타 스레드에서 시야 처리 안 된경우.
//				{
//					ReleaseSRWLockShared(&new_actor.second->lock);
//
//					new_actor.second->send_enter_packet(this);
//				}
//				else // 처리 된경우
//				{
//					ReleaseSRWLockShared(&new_actor.second->lock);
//
//					new_actor.second->send_move_packet(this);   // 상대 시야 리스트에 내가 있는 경우 뷰리스트만 업데이트 한다.
//				}
//			}
//			else  // 기존 뷰리스트에 있던 유저들 
//			{
//				AcquireSRWLockShared(&new_actor.second->lock);					//
//				if (0 != new_actor.second->view_list_for_npc.count(this->id))	// 
//				{
//					ReleaseSRWLockShared(&new_actor.second->lock);
//
//					new_actor.second->send_move_packet(this);
//				}
//				else	// 이미 나간 경우.
//				{
//					ReleaseSRWLockShared(&new_actor.second->lock);
//
//					new_actor.second->send_enter_packet(this);
//				}
//			}
//
//			if (true == is_near(new_actor.second))
//			{
//				set_target(new_actor.second);
//			}
//		}
//
//
//		//시야에서 벗어난 플레이어
//		for (auto& old_it : local_old_view_list)
//		{
//			if (0 == local_new_view_list.count(old_it.first))		// 현재 내 시야에 없는 플레이어.
//			{														// 
//				this->update_leaving_actor(old_it.second);			// npc 시야에서 유저가 나간것을 업뎃 해줌.
//
//				AcquireSRWLockShared(&old_it.second->lock);			// 
//				if (0 != old_it.second->view_list_for_npc.count(this->id)) // 클라 시야에 내가 있는 경우
//				{
//					ReleaseSRWLockShared(&old_it.second->lock);
//					old_it.second->send_leave_packet(this);
//				}
//				else	// 이미 다른 스레드에서 지워준 경우.
//				{
//					ReleaseSRWLockShared(&old_it.second->lock);
//				}
//			}
//		}
//
//		if (is_isolated == true)
//		{
//			is_active = NPC_SLEEP;
//			//target = nullptr;
//			has_target = false;
//			// 최후 확인용 작업 // 이작업중에 누가 
//		}
//		else // 있다면 업데이트 계속.
//		{
//			local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
//		}
//	}
//	else
//	{
//		is_active = NPC_SLEEP;
//		//target = nullptr;
//		has_target = false;
//	}
//}
