#include "protocol.h"
#include "MMOSession.h"
#include "MMOServer.h"

#include "DBHelper.h"
#include "db_statement.h"

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

void MMOSession::has_vliad_id()
{
	DbHelper	db_helper;
	int			id, level, hp, exp, y, x;
	wchar_t		name[100];
	wchar_t		password[100];

	int uid = 100;
	db_helper.bind_param_int(&uid);

	db_helper.bind_result_column_int(&id);
	db_helper.bind_result_column_text(name, count_of(name));
	db_helper.bind_result_column_int(&y);
	db_helper.bind_result_column_int(&x);
	db_helper.bind_result_column_int(&level);
	db_helper.bind_result_column_int(&exp);
	db_helper.bind_result_column_int(&hp);

	if (db_helper.execute(sql_load_actor))
	{
		if (true == db_helper.fetch_row())
		{
			wprintf(L"%d %d %d %d %d, %d| \n", id, level, y, x, hp, exp);
		}
	}
}
