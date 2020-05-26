#include "../C2Server/C2Server/pre_compile.h"
#include "MMOServer.h"
#include "MMOZone.h"
#include "MMOMessage.h"
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
				printf("c2s_login");
				cs_packet_login		in_payload;
				sc_packet_login_ok	out_payload;

				msg.in_packet->read(&in_payload, sizeof(in_payload));

				if (msg.session_id != msg.session->session_id)
				{
					c2::Packet::release(msg.in_packet); // 허허...  이미 나간 녀석.

					// log
					break;
				}
				//c2::Packet::release(msg.in_packet);
				// DB 요청.... break;

////////	////////////////////////////// 하나더 나누는거임.
						// zone에 접속....
				MMOActor* actor = msg.session->get_actor();
				actor->reset();
				actor->set_name(in_payload.name); // 
				zones->accept_actor(actor);


				msg.in_packet->clear(); /// 귀찮아서 일단 여기서 보내고 잇음.
				actor->get_login_packet_info(out_payload); // db처리로 비동기로 바꿔야 함;
				msg.in_packet->write(&out_payload, sizeof(sc_packet_login_ok)); // 재활용 
				server->send_packet(msg.session_id, msg.in_packet); // 클라에게 통지 요청.

				break;
			}

			case c2::enumeration::C2S_MOVE:
			{
				cs_packet_move in_payload;
				msg.in_packet->read(&in_payload, sizeof(in_payload));

				// id check
				MMOActor* actor = msg.session->get_actor();
				actor->set_move_time(in_payload.move_time);
				actor->move(in_payload.direction);
				break;
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

void MMOSimulator::simulate()
{
	// 변환점을 확인하고 update
	zones->simulate_interaction();
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
