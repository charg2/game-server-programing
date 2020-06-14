#include "protocol.h"
#include "MMOSession.h"
#include "MMOServer.h"

#include "db/DBHelper.h"
#include "MMODBTask.h"
#include "db/DBManager.h"
#include "MMOZone.h"
#include "MMONpcManager.h"

MMOSession::MMOSession() : actor{ this }
{}

MMOSession::~MMOSession()
{}

MMOActor* MMOSession::get_actor()
{
	return &actor;
}

void MMOSession::response_loginok()
{
	sc_packet_login_ok loginok_payload;  // 주변에 보내기 위한 내정보 // 나중엔 DB 요청으로 변경.... break;
	
	actor.get_login_packet_info(loginok_payload);						// db처리로 비동기로 바꿔야 함;


	c2::Packet* loginok_packet = c2::Packet::alloc();						

	loginok_packet->write(&loginok_payload, sizeof(sc_packet_login_ok));	//
	server->send_packet(session_id, loginok_packet);			// 클라에게 통지 알려줌 접속을
}

void MMOSession::response_login_failure()
{
	sc_packet_login_ok loginok_payload;  // 주변에 보내기 위한 내정보 // 나중엔 DB 요청으로 변경.... break;
	actor.get_login_packet_info(loginok_payload);						// db처리로 비동기로 바꿔야 함;

	loginok_payload.id = -1;

	c2::Packet* loginok_packet = c2::Packet::alloc();

	loginok_packet->write(&loginok_payload, sizeof(sc_packet_login_ok));	//

	server->send_packet(session_id, loginok_packet);			// 클라에게 통지 알려줌 접속을
}

void MMOSession::on_handling_db_task(DBTask* task)
{
	switch (task->type)
	{
		case LOAD_ACTOR:
		{
			LoadActorTask* load_task = reinterpret_cast<LoadActorTask*>(task);
			if (load_task->is_success == true)
			{
				actor.set_data(load_task);

				MMOZone* zone = g_server->get_zone();
				
				this->response_loginok();		// 로그인 처리 및 응답

				zone->enter_actor(&actor);

				MMOSector* current_sector = zone->get_sector(&actor);
				const MMONear* nears = current_sector->get_near(actor.y, actor.x); // 주벽 섹터들.
				int				near_cnt = nears->count;


				std::unordered_map<int32_t, MMOActor*>	local_view_list;
				std::unordered_set<int32_t>				local_view_list_for_npc;


				for (int n = 0; n < near_cnt; ++n)
				{
					AcquireSRWLockShared(&nears->sectors[n]->lock); //sector에 읽기 위해서 락을 얻고 
					for (auto& other_iter : nears->sectors[n]->actors)
					{
						if (other_iter.second == &actor)
							continue;

						if (actor.is_near(other_iter.second) == true) // 근처가 맞다면 넣음.
							local_view_list.insert(other_iter);
						//mmo_actor->view_list.insert(other_iter);
					}

					//NPC 처리 로직.
					for (auto other_iter : nears->sectors[n]->npcs)
					{
						MMONpc* npc = g_npc_manager->get_npc(other_iter);
						if (actor.is_near(npc) == true) // 근처가 맞다면 넣음.
						{
							//mmo_actor->view_list_for_npc.insert(other_iter);			// 배 npc용 시야 리스트에 넣어줌.
							local_view_list_for_npc.insert(other_iter);			// 배 npc용 시야 리스트에 넣어줌.
						}
					}
					ReleaseSRWLockShared(&nears->sectors[n]->lock);
				}



				////////// 내정보를 상대방에 보내고 나는 상대방 정보를 받는다. 
				c2::Packet* my_info_packet = c2::Packet::alloc();				// 주변에 보내기 위한 내정보 
				sc_packet_enter my_info_payload{ {sizeof(sc_packet_enter), S2C_ENTER}, (int16_t)this->session_id, {}, 0, actor.x , actor.y };
				memcpy(my_info_payload.name, actor.name, 50);
				my_info_packet->write(&my_info_payload, sizeof(sc_packet_enter));


				//my_info_packet->add_ref( local_view_list.size() );			// 락 밖에서 하면 언제  사이즈가 변경되어 있을지 모름.
				for (auto& iter : local_view_list)
				{
					if (iter.second == &actor)
						continue;

					MMOActor* other = iter.second;
					actor.send_enter_packet_without_updating_viewlist(other);
					other->send_enter_packet(&actor, my_info_packet);
				}

				my_info_packet->decrease_ref_count();						// my_info_packet 릴리즈용 


			////// NPC 처리.
				for (auto npc_id : local_view_list_for_npc)
				{
					MMONpc* npc = g_npc_manager->get_npc(npc_id);
					AcquireSRWLockExclusive(&npc->lock);						//내 view_list 에 접근하기 쓰기 위해서 락을 얻고 
					npc->view_list.emplace(actor.get_id(), &actor);		// 서로 시야 리스트에 넣어줌.
					ReleaseSRWLockExclusive(&npc->lock);						//내 view_list 에 접근하기 쓰기 위해서 락을 얻고 

					local_timer->push_timer_task(npc->id, TTT_NPC_SCRIPT, 1, actor.session_id);

					// move에서도./// 나한테 npc 정보 보내기 하셈. 
					actor.send_enter_packet_without_updating_viewlist(npc);
				}


				AcquireSRWLockExclusive(&actor.lock);							//내 view_list 에 접근하기 읽기 위해서 락을 얻고 
				actor.view_list = local_view_list;
				actor.view_list_for_npc = local_view_list_for_npc;
				ReleaseSRWLockExclusive(&actor.lock);
			}
			else
			{
				this->response_login_failure();		// 로그인 처리 및 응답
			}
			break;
		}
		case DTT_CREATE_ACTOR:
		{
			// 락을 걸고 반영함.
			// 메시지 보고 .


		}
			break;
	}

	delete task;
}

void MMOSession::request_login_validation(char* name)
{
	LoadActorTask* load_actor_task = new LoadActorTask(this->session_id, name);
	
	g_db_manager->post_db_reading_task(load_actor_task);
}

void MMOSession::request_updating_position(int y, int x)
{
	UpdatePositionTask* task = new UpdatePositionTask(this->session_id, actor.user_id, y, x);
	
	g_db_manager->post_db_writing_task(task);
}


//
//
//void MMOSession::has_vliad_id()
//{
//	DbHelper	db_helper;
//	int			id, level, hp, exp, y, x;
//	wchar_t		name[100];
//	wchar_t		password[100];
//
//	int uid = 100;
//	db_helper.bind_param_int(&uid);
//
//	db_helper.bind_result_column_int(&id);
//	db_helper.bind_result_column_wstr(name, count_of(name));
//	db_helper.bind_result_column_int(&y);
//	db_helper.bind_result_column_int(&x);
//	db_helper.bind_result_column_int(&level);
//	db_helper.bind_result_column_int(&exp);
//	db_helper.bind_result_column_int(&hp);
//
//	if (db_helper.execute(sql_load_actor))
//	{
//		if (true == db_helper.fetch_row())
//		{
//			wprintf(L"%d %d %d %d %d, %d| \n", id, level, y, x, hp, exp);
//		}
//	}
//}
