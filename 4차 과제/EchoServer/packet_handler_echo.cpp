#include "../C2Server/C2Server/packet_handler.h"
#include "../C2Server/C2Server/OuterServer.h"
#include "EchoSession.h"


REGISTER_HANDLER(PT_CS_ECHO)
{
	EchoSession* echo_session = (EchoSession*)session;

	auto out_packet  = c2::Packet::alloc();

	out_packet->write(in_packet.get_buffer(), in_packet.size());

	echo_session->server->send_packet(echo_session->session_id, out_packet);
}
