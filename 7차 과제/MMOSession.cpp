#include "protocol.h"
#include "MMOSession.h"
#include "MMOServer.h"

MMOSession::MMOSession() : actor{ this }
{
}

MMOSession::~MMOSession()
{
}

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
