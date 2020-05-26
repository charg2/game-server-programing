#include "../C2Server/C2Server/packet_handler.h"
#include "../C2Server/C2Server/OuterServer.h"
#include "MMOSession.h"
#include "MMOSimulator.h"


REGISTER_HANDLER(C2S_LOGIN)
{
	MMOSimulator* simulator = &MMOSimulator::get_instance();

	MMOMessage message;
	message.in_packet = c2::Packet::alloc();
	message.session = (MMOSession*)session;
	message.session_id = session->session_id;
	message.type = C2S_LOGIN;

	message.in_packet->write(in_packet.get_buffer(), in_packet.size());

	simulator->put_message(&message);
}

REGISTER_HANDLER(C2S_MOVE)
{
	static MMOSimulator& simulator = MMOSimulator::get_instance();

	MMOMessage message;
	message.in_packet = c2::Packet::alloc();
	message.session = (MMOSession*)session;
	message.session_id = session->session_id;
	message.type = C2S_MOVE;

	message.in_packet->write(in_packet.get_buffer(), in_packet.size());

	simulator.put_message(&message);
}


REGISTER_HANDLER(C2S_CHAT)
{
	static MMOSimulator& simulator = MMOSimulator::get_instance();

	MMOMessage message;
	message.in_packet = c2::Packet::alloc();
	message.session = (MMOSession*)session;
	message.session_id = session->session_id;
	message.type = C2S_CHAT;

	message.in_packet->write(in_packet.get_buffer(), in_packet.size());

	simulator.put_message(&message);
}
