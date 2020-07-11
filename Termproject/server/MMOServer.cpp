//#include "pre_compile.h"


#include "main.h"
#include "MMOServer.h"
#include "MMONpcManager.h"
#include "util/TimeScheduler.h"
#include "contents_enviroment.h"
#include "MMOZone.h"


MMOServer::MMOServer() : OuterServer{}, zone{}
{
	g_zone = new MMOZone{};
	zone = g_zone;
	g_zone->server = this;
}

MMOServer::~MMOServer()
{
	delete g_zone;
}

void MMOServer::init_npcs()
{
	g_npc_manager = new MMONpcManager();
	g_npc_manager->set_zone(zone);
	g_npc_manager->initilize();
	g_npc_manager->place_npc_in_zone();
}

void MMOServer::on_create_sessions(size_t capacity)
{
	session_mapping_helper(MMOSession);
}

void MMOServer::on_connect(uint64_t session_id){}

void MMOServer::on_disconnect(uint64_t session_id) 
{
	MMOActor*	my_actor = get_actor(session_id);
	my_actor->session->request_updating_all( my_actor->y, my_actor->x, my_actor->hp, my_actor->level, my_actor->current_exp);
	
	int			my_actor_id = my_actor->get_id();

	
	MMOSector*	my_actor_sector = my_actor->current_sector;
	if (my_actor_sector == nullptr)
	{
		return;
	}
	
	AcquireSRWLockExclusive(&my_actor_sector->lock);
	my_actor_sector->actors.erase(my_actor_id);
	ReleaseSRWLockExclusive(&my_actor_sector->lock);

	AcquireSRWLockExclusive(&my_actor->lock);
	auto local_view_list = my_actor->view_list;
	ReleaseSRWLockExclusive(&my_actor->lock);

	//auto& view_list = my_actor->view_list;

	for( auto& actor_iter : local_view_list)
	{
		//if (actor_iter.second == my_actor)
		//{
		//	actor_iter.second->send_leave_packet_without_updating_viewlist(my_actor); // 이미 락을 걸어놓으애ㅕ서 
		//	continue;
		//}

		//AcquireSRWLockExclusive(&actor_iter.second->lock); // 데드락 가능성 있다;
		//if (0 != actor_iter.second->view_list.count(my_actor_id))
		//{
			//ReleaseSRWLockExclusive(&actor_iter.second->lock);
			actor_iter.second->send_leave_packet(my_actor);
		//}
		//else
		//{
			//ReleaseSRWLockExclusive(&actor_iter.second->lock);
		//}
	}
	

	//ReleaseSRWLockExclusive(&my_actor->lock);
}

void MMOServer::on_wake_io_thread(){}
void MMOServer::on_sleep_io_thread(){}
void MMOServer::custom_precedure(uint64_t idx) {}
void MMOServer::on_update(){}
void MMOServer::on_start()
{
	for (;;)
	{
		//if (km.key_down(VK_RETURN))
		//{
			//printf("hi\n");
		//}

		Sleep(30);
	}

	return;
}

//
void MMOServer::on_timer_service(const TimerTask& task)
{
	switch (task.task_type)
	{
		case TTT_ON_WAKE_FOR_NPC:
		{
			MMONPC* npc = g_npc_manager->get_npc(task.actor_id);
			
			local_timer->push_timer_task(task.actor_id, TTT_UPDATE_FOR_NPC, 0, task.target_id );

			break;
		}
		case TTT_UPDATE_FOR_NPC:
		{
			MMONPC* npc = g_npc_manager->get_npc(task.actor_id);

			switch (npc->type)
			{
			case NT_PEACE_FIXED:
				npc->update_for_fixed_peace();
				npc->attack_for_peace();
				break;

			case NT_PEACE_ROAMING:
				if (npc->has_target == false)
				{
					npc->move_to_anywhere();
				}
				else
				{
					npc->update_for_peace();
					npc->attack_for_peace();
				}
				break;

			case NT_COMBAT_FIXED:
				npc->update_for_fixed_combat();
				npc->attack_for_peace();
				break;;

			case NT_COMBAT_ROAMING:
				if (npc->has_target == false)
				{
					npc->move_to_anywhere();
				}
				else
				{
					npc->update_for_combat();
					npc->attack_for_peace();
				}
				
				break;
			default:
				printf("invalid obj type");
				break;
			}

			break;
		}

		case TTT_ON_SLEEP_FOR_NPC:
		{
			MMONPC* npc = g_npc_manager->get_npc(task.actor_id);

			break;
		}
		case TTT_USER_RECOVER_HP:
		{
			auto* actor = g_server->get_actor(task.actor_id);
			if (actor->is_alive == true)
			{
				actor->increase_hp(10);

				if (actor->hp > 0)
				{
					local_timer->push_timer_task(actor->session_id, TTT_USER_RECOVER_HP, 10 * 1000, NULL);
				}
			}
			break;
		}
		case TTT_RESPAWN_FOR_PLAYER: // 한명만 ㅇㅇ.
		{
			MMOActor* user = g_server->get_actor(task.actor_id);
			user->respawn();

			break;
		}
		case TTT_RESPAWN_FOR_NPC: // 한명만 ㅇㅇ.
		{
			MMONPC* npc = g_npc_manager->get_npc(task.actor_id);
			
			npc->reset();
			
			npc->respawn();

			break;
		}

		default:
		{
			size_t* invliad_ptr{}; *invliad_ptr = 0;
		}
	}
}

bool MMOServer::on_load_config(c2::util::JsonParser* parser)
{
	uint32_t max_npc{};
	if (false == parser->get_uint32(L"maximum_npc_count", max_npc))
		return false;

	MMONpcManager::set_max_npc_count(max_npc);

	if (false == parser->get_uint32(L"maximum_npc_count", c2::global::max_npc))
		return false;
	
	if (false == parser->get_raw_wstring(L"db_connection_string", c2::global::db_connection_string, count_of(c2::global::db_connection_string)))
		return false;

	if (false == parser->get_int32(L"db_thread_count", c2::global::db_read_thread_count))
		return false;

	c2::global::concurrent_db_reader_thread_count = c2::global::db_read_thread_count;

	if (false == parser->get_raw_wstring(L"db_server_name", c2::global::db_server_name, count_of(c2::global::db_server_name)) )
		return false;



////////////////////////////////
	parser->load_json(L"mmo_map.json");

	if (false == parser->get_int32(L"mmo_map_width", c2::global::obstacle_table_width))
		return false;

	int height{};
	if (false == parser->get_int32(L"mmo_map_height", c2::global::obstacle_table_height))
		return false;

	if (false == parser->get_map(L"mmo_map", c2::global::obstacle_table, c2::global::obstacle_table_width, c2::global::obstacle_table_height))
		return false;

	return true;
}

void MMOServer::create_npcs(size_t capacity)
{

}

MMOActor* MMOServer::get_actor(uint64_t session_id)
{
	return ((MMOSession*)sessions[(uint16_t)session_id])->get_actor();
}

MMOZone* MMOServer::get_zone()
{
	return zone;
}


//
//void MMONPC::update_for_fixed_peace()
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
//	if (false == has_target || false == is_in_attack_range(local_target)) // 대상이 없거나 거리가 멀어지면 종료.
//	{
//		is_active = NPC_SLEEP;
//		has_target = false;
//
//		return;
//	}
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
//
//					break;
//				}
//				else
//				{
//					local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
//					return;
//				}
//			}
//		}
//
//		if (true == has_obstacle)  // 장애물이 있다면 astar로 길 찾기.
//		{
//			printf("astar");
//
//			if (true == PathFindingHelper->NewPath(local_x, local_y, local_target_x, local_target_y))
//			{
//				if (true == PathFindingHelper->PathNextNode())
//				{
//					printf("---ok (%d, %d) ", local_x, local_y);
//					x = local_x = PathFindingHelper->NodeGetX();
//					y = local_y = PathFindingHelper->NodeGetY();
//					printf(" -- (%d, %d) \n", local_x, local_y);
//				}
//			}
//		}
//
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
//		// 주변에 뿌리기.
//		MMOSector* current_sector = new_sector;
//		const MMONear* nears = current_sector->get_near(local_y, local_x);
//		int				near_cnt = nears->count;
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
//			if (true == is_in_attack_range(new_actor.second))
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
