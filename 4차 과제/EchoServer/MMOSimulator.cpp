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
}

MMOSimulator& MMOSimulator::get_instance()
{
	static MMOSimulator instance;
	return instance;
}

void MMOSimulator::dispatch()
{
	MMOMessage msg;

	while (message_queue->try_pop(msg))
	{
		switch (msg.type)
		{
			case c2::enumeration::C2S_LOGIN:
			{
				printf(" c2s_login \n");

				cs_packet_login		in_payload;
				sc_packet_login_ok	out_payload;

				msg.in_packet->read(&in_payload, sizeof(in_payload));

				if (msg.session_id != msg.session->session_id)
				{
					c2::Packet::release(msg.in_packet); // ����...  �̹� ���� �༮.

					// log
					break;
				}
				//c2::Packet::release(msg.in_packet);
				// DB ��û.... break;

////////	////////////////////////////// �ϳ��� �����°���.
						// zone�� ����....
				MMOActor* actor = msg.session->get_actor();
				actor->reset();
				actor->set_name(in_payload.name); // 
				zones->accept_actor(actor);


				msg.in_packet->clear(); /// �����Ƽ� �ϴ� ���⼭ ������ ����.
				actor->get_login_packet_info(out_payload); // dbó���� �񵿱�� �ٲ�� ��;
				msg.in_packet->write(&out_payload, sizeof(sc_packet_login_ok)); // ��Ȱ�� 
				server->send_packet(msg.session_id, msg.in_packet); // Ŭ�󿡰� ���� ��û.

				break;
			}

			case c2::enumeration::C2S_MOVE:
			{
				cs_packet_move in_payload;
				msg.in_packet->read(&in_payload, sizeof(in_payload));

				// id check
				MMOActor* actor = msg.session->get_actor();
				actor->move(in_payload.direction);
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
	// ��ȯ���� Ȯ���ϰ� update
	zones->simulate_interaction();
}

void MMOSimulator::put_message(MMOMessage* message)
{
	message_queue->push(*message);
}

void MMOSimulator::set_server(MMOServer* server)
{
	this->server = server;
}
