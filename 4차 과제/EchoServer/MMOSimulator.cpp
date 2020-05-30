#include "../C2Server/C2Server/pre_compile.h"
#include "MMOServer.h"
#include "MMOZone.h"
#include "MMOMessage.h"
#include "MMONear.h"
#include "MMOSimulator.h"
#include "Timer.h"

MMOSimulator::MMOSimulator() : zones{}
{
	zones = new MMOZone();
	message_queue = new MPSCQueue<MMOMessage>();
	disconnected_session_id = new MPSCQueue<uint64_t>();
}

MMOSimulator& MMOSimulator::get_instance()
{
	static MMOSimulator instance;
	return instance;
}

void MMOSimulator::dispatch()
{
	MMOMessage msg;
	uint64_t session_id;

	while (disconnected_session_id->try_pop(session_id))
	{
		zones->erase_session(session_id);
	}

	while (message_queue->try_pop(msg))
	{
		switch (msg.type)
		{
			case c2::enumeration::C2S_LOGIN:
			{
				cs_packet_login		in_payload;
				sc_packet_login_ok	out_payload;
				msg.lonin_ok_packet->read(&in_payload, sizeof(in_payload));
				if (msg.session_id != msg.session->session_id)
				{
					msg.lonin_ok_packet->decrease_ref_count(); // 허허...  이미 나간 녀석.

					break;
				}

				// zone에 접속....
				MMOActor* actor = msg.session->get_actor();
				actor->reset();
				actor->set_name(in_payload.name); // 
				msg.lonin_ok_packet->clear(); /// 귀찮아서 일단 여기서 보내고 잇음.
				actor->get_login_packet_info(out_payload); // db처리로 비동기로 바꿔야 함;
				msg.lonin_ok_packet->write(&out_payload, sizeof(sc_packet_login_ok)); // 재활용 
				server->send_packet(msg.session_id, msg.lonin_ok_packet); // 클라에게 접속 알림.


				MMOSector* cur_sector = zones->get_sector(actor->y, actor->x);
				cur_sector->actors.insert(std::make_pair(actor->get_id(), actor));


				sc_packet_enter my_info_payload;							// 내 정보 구조체 생성 초기화 
				my_info_payload.header.length = sizeof(sc_packet_enter);
				my_info_payload.header.type = S2C_ENTER; // header
				my_info_payload.id = actor->get_id();
				memcpy(my_info_payload.name, actor->name, 50);
				//my_info_payload.o_type; // 아마 오브젝트 타입인듯.
				my_info_payload.x = actor->x; my_info_payload.y = actor->y;


				c2::Packet* my_info_packet = c2::Packet::alloc();
				my_info_packet->write(&my_info_payload, sizeof(sc_packet_enter));// 내정보 타인한테 보내기
				//server->send_packet(actor->session_id, my_info_packet); // 일단 나한테 보내기;




				// 타인 정보
				sc_packet_enter other_info_payload;
				other_info_payload.header.length = sizeof(sc_packet_enter);
				other_info_payload.header.type = S2C_ENTER;


				//  주변 섹터 구해서 
				MMONear* near_sectors = zones->get_sector(actor->y, actor->x)->get_near(actor->y, actor->x);
				int cnt = near_sectors->count;
				for (int n = 0; n < cnt; ++n)
				{
					auto near_actors = near_sectors->sectors[n]->actors; // 섹터별 인원 구해서

					for (auto& it : near_actors) // 주변 인원들에게 내 정보 페킷 만들어서 발사.
					{
						MMOActor* other = it.second;

						if (actor == other) continue;
						if (actor->is_near(other) == false) continue;

						// 뷰리스트 업뎃
						actor->view_list.emplace(other->get_id(), other);
						other->view_list.emplace(actor->get_id(), actor);

						////
						other_info_payload.id = other->get_id();
						memcpy(other_info_payload.name, other->name, sizeof(sc_packet_enter::name));
						other_info_payload.x = other->get_x();
						other_info_payload.y = other->get_y();


						// 타인정보 나한테 보내기
						c2::Packet* other_info_packet = c2::Packet::alloc();
						other_info_packet->write(&other_info_payload, sizeof(sc_packet_enter));
						server->send_packet(actor->session_id, other_info_packet); // 타인정보 나한테 보내기

						// 내정보 주변에 보내기.
						my_info_packet->increase_ref_count();
						server->send_packet(other->session_id, my_info_packet); // 내정보 남한테 보내기
					}
				}
				

				
				/// 마지막으로 sector 반영.
				actor->prev_sector = actor->current_sector;
				actor->current_sector = zones->get_sector(actor->y, actor->x);
				break;
			}

			case c2::enumeration::C2S_MOVE:
			{
				cs_packet_move in_payload;
				msg.lonin_ok_packet->read(&in_payload, sizeof(in_payload));

				// id check
				MMOActor* actor = msg.session->get_actor();
				actor->zone = zones;
				actor->set_move_time(in_payload.move_time);

				MMOSector* old_cur_sector = zones->get_sector(actor->y, actor->x);
				actor->move(in_payload.direction);
				MMOSector* new_cur_sector = zones->get_sector(actor->y, actor->x);
				if (old_cur_sector != new_cur_sector)
				{
					old_cur_sector->actors.erase(actor->get_id());
					new_cur_sector->actors.emplace(actor->get_id(), actor);
				}
				
				sc_packet_leave my_leave_payload;
				my_leave_payload.header.length = sizeof(sc_packet_leave);
				my_leave_payload.header.type = S2C_LEAVE;
				my_leave_payload.id = actor->get_id();

				sc_packet_move my_move_payload;
				my_move_payload.header.length = sizeof(sc_packet_move);
				my_move_payload.header.type = S2C_MOVE;
				my_move_payload.x = actor->x;
				my_move_payload.y = actor->y;
				my_move_payload.id = actor->get_id();
				my_move_payload.move_time = actor->last_move_time;


				// 내정보 타인한테 보내기.
				sc_packet_enter my_enter_payload;
				my_enter_payload.header.length = sizeof(sc_packet_enter);
				my_enter_payload.header.type = S2C_ENTER;
				my_enter_payload.id = actor->get_id();
				memcpy(my_enter_payload.name, actor->name, sizeof(sc_packet_enter::name));
				my_enter_payload.x = actor->x;
				my_enter_payload.y = actor->y;


				sc_packet_leave other_leave_payload;
				other_leave_payload.header.length = sizeof(sc_packet_leave);
				other_leave_payload.header.type = S2C_LEAVE;

				sc_packet_enter other_enter_payload;
				other_enter_payload.header.length = sizeof(sc_packet_enter);
				other_enter_payload.header.type = S2C_ENTER;

				sc_packet_move other_move_payload;
				other_move_payload.header.length = sizeof(sc_packet_move);
				other_move_payload.header.type = S2C_MOVE;
				
				c2::Packet* my_enter_packet = c2::Packet::alloc(); // my enter 패킷.
				c2::Packet* my_leave_packet = c2::Packet::alloc();	// my leave 패킷.
				c2::Packet* my_move_packet = c2::Packet::alloc(); // my move 패킷.
				
				my_enter_packet->write(&my_enter_payload, sizeof(sc_packet_enter));
				my_leave_packet->write(&my_leave_payload, sizeof(sc_packet_leave));
				my_move_packet->write(&my_move_payload, sizeof(sc_packet_move));

				server->send_packet(actor->session_id, my_move_packet);
				// leave 보낼 곳.

				MMONear* near_sectors = actor->current_sector->get_near(actor->y, actor->x);
				int cnt = near_sectors->count;

				for (int n = 0; n < cnt; ++n)
				{
					auto& actors = near_sectors->sectors[n]->actors;
					for (auto& it : actors)
					{
						MMOActor* other = it.second;

						if ( actor == other ) continue;
						if ( 0 == actor->view_list.count(it.first)) // 주변에 있고 시야에 없었다면 새로 들어온 유저.
						{
							other_enter_payload.id = it.second->get_id(); // 타인 패킷 완성
							memcpy(other_enter_payload.name, it.second->name, sizeof(sc_packet_enter::name));
							other_enter_payload.x = other->x;
							other_enter_payload.y = other->y;

							c2::Packet* other_enter_packet = c2::Packet::alloc();
							other_enter_packet->write(&other_enter_payload, sizeof(sc_packet_enter));

							my_enter_packet->increase_ref_count();
							server->send_packet(it.second->session_id, my_enter_packet);// 남한테 나들간거 보내기.
							server->send_packet(actor->session_id, other_enter_packet); // 나한테 타인 정보 보내기.


							// 뷰리스트 정리.
							actor->view_list.emplace(it);
							it.second->view_list.emplace(actor->get_id(), actor);
						}
						else  // 기존에 있던 유저
						{
							// 남한테 움직이는거 보내기.
							my_move_packet->increase_ref_count();
							server->send_packet(it.second->session_id, my_move_packet);
							
							//other_move_payload.move_time	= it.second->last_move_time ;
							//other_move_payload.x			= it.second->x ;
							//other_move_payload.y			= it.second->y ;

							//c2::Packet* other_move_packet = c2::Packet::alloc();
							//other_move_packet->write(&other_move_payload, sizeof(sc_packet_move));
							////// 나한테 남 움직이는거. // 안해도 될듯
							//server->send_packet(actor->session_id, other_move_packet);
						}
					}
				}


				// leave 보낼곳.
				for (auto& it : actor->view_list)
				{
					if (actor == it.second) continue;
					if (actor->is_near(it.second) == false)
					{
						other_leave_payload.id = it.second->get_id(); // 타인 패킷 완성
						c2::Packet* other_leave_packet = c2::Packet::alloc();
						other_leave_packet->write(&other_leave_payload, sizeof(sc_packet_leave));

						// 남한테 나나간거 보내기.
						my_leave_packet->increase_ref_count();
						server->send_packet(it.second->session_id, my_leave_packet);
						// 나한테 남나간거 보내기.
						server->send_packet(actor->session_id, other_leave_packet);

						// 뷰리스트 정리.
						it.second->view_list.erase(actor->get_id());
						actor->view_list.erase(it.second->get_id());		
					}
				}

				my_leave_packet->decrease_ref_count();


				// enter 보낼 곳.

				
				actor->last_move_time = 0;
				return;
			}

			case c2::enumeration::C2S_CHAT:
			{
				//cs_packet_chat chat_payload;
				//msg.in_packet->read(&chat_payload, sizeof(cs_packet_chat));

				//// id check
				//MMOActor* actor = msg.session->get_actor();
				//MMOSector* sector = actor->get_current_sector();
				//std::vector<MMOSector*>& near_sectors =sector->get_near_sectors();
				//for ( MMOSector* n_sector : near_sectors)
				//{
				//	auto& actors = n_sector->get_actors();
				//	for (auto& iter : actors)
				//	{
				//		MMOActor* neighbor = iter.second;
				//		c2::Packet* out_packet =  c2::Packet::alloc();
				//		out_packet->write(&chat_payload, sizeof(cs_packet_chat));
				//		server->send_packet(neighbor->get_session_id(), out_packet);
				//	}
				//}

				break;
			}
			default:
			{
				c2::util::crash_assert();
				break;
			}
		}
	}
}


void MMOSimulator::put_message(MMOMessage* message)
{
	message_queue->push(*message);
}

void MMOSimulator::put_disconnected_session(uint64_t session_id)
{
	disconnected_session_id->push(session_id);
}

void MMOSimulator::set_server(MMOServer* server)
{
	this->server = server;
}

MMOServer* MMOSimulator::get_server()
{
	return server;
}
