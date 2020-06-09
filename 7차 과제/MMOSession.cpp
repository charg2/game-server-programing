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
	sc_packet_login_ok loginok_payload;  // �ֺ��� ������ ���� ������ // ���߿� DB ��û���� ����.... break;
	
	actor.get_login_packet_info(loginok_payload);						// dbó���� �񵿱�� �ٲ�� ��;
	
	c2::Packet* loginok_packet = c2::Packet::alloc();						

	loginok_packet->write(&loginok_payload, sizeof(sc_packet_login_ok));	//
	server->send_packet(session_id, loginok_packet);			// Ŭ�󿡰� ���� �˷��� ������
}
