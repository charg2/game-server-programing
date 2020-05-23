#include "../C2Server/C2Server/packet_handler.h"
#include "../C2Server/C2Server/OuterServer.h"
#include "MMOSession.h"
#include "MMOSimulator.h"


REGISTER_HANDLER(C2S_LOGIN)
{
	MMOSimulator& simulator = MMOSimulator::get_instance();
	//simulator.

}

REGISTER_HANDLER(C2S_MOVE)
{
	MMOSession* echo_session = (MMOSession*)session;

	auto out_packet = c2::Packet::alloc();

	out_packet->write(in_packet.get_buffer(), in_packet.size());

	echo_session->server->send_packet(echo_session->session_id, out_packet);
}
