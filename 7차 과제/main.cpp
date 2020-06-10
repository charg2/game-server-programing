#include "main.h"
#include "MMOServer.h"
#include "DBHelper.h"
#include "contents_enviroment.h"

/*
서버
- IOCP
- 맵
- 몬스터
- NPC
- 길찾기
- DB
- SCRIPT
- 채팅
- 장애물
- 버프
- FSM
- 1초에 한칸

클라
- HP 레벨 표시
- 메시지 창 ( 채팅 가능 )
- 알림 창
*/

#include <memory>

using namespace c2;
#include "db_statement.h"
void main()
{
	setlocale(LC_ALL, "");

	g_server = new MMOServer();

	g_server->setup_dump();

	g_server->load_config_using_json(L"config.json");

	if (false == DbHelper::initialize(global::db_connection_string, global::concurrent_io_thread_count))
		return;


	g_server->init_npcs();

	g_server->initialize();

	g_server->start();



	//c2::local::io_thread_id = 0;

	//{
	//	DbHelper db_helper;
	//	int id, level, hp, exp, y, x;
	//	wchar_t name[100];
	//	wchar_t password[100];

	//	int uid = 100;
	//	db_helper.bind_param_int(&uid);

	//	db_helper.bind_result_column_int(&id);
	//	db_helper.bind_result_column_text(name, count_of(name));
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
	//			wprintf(L"%d %d %d %d %d, %d| \n", id,  level, y, x, hp, exp);
	//		}
	//	}
	//}


	//{
	//	DbHelper db_helper;
	//	int id, level, hp, exp, y, x;
	//	wchar_t name[100];
	//	wchar_t password[100];

	//	int uid = 101;
	//	db_helper.bind_param_int(&uid);

	//	db_helper.bind_result_column_int(&id);
	//	db_helper.bind_result_column_text(name, count_of(name));
	//	db_helper.bind_result_column_int(&y);
	//	db_helper.bind_result_column_int(&x);
	//	db_helper.bind_result_column_int(&level);
	//	db_helper.bind_result_column_int(&exp);
	//	db_helper.bind_result_column_int(&hp);

	//	if (db_helper.execute(sql_load_actor))
	//	{
	//		if (true == db_helper.fetch_row())
	//		{
	//			wprintf(L"%d %d %d %d %d, %d| \n", id, level, y, x, hp, exp);
	//		}
	//	}
	//}


	//{
	//	DbHelper db_helper;

	//	int uid = 101, y = 259, x = 144;
	//	db_helper.bind_param_int(&uid);
	//	db_helper.bind_param_int(&y);
	//	db_helper.bind_param_int(&x);
	//	if (true == db_helper.execute(sql_update_actor_position))
	//	{
	//		if (true == db_helper.fetch_row())
	//		{
	//			wprintf(L"actor position update ok\n");
	//		}
	//	}
	//}

	//{
	//	DbHelper db_helper;
	//	const wchar_t* name{ L"actor_21" };
	//	wchar_t password[100];

	//	db_helper.bind_param_text(name);
	//	if (true == db_helper.execute(sql_create_actor))
	//	{
	//		if (true == db_helper.fetch_row())
	//		{
	//			wprintf(L"actor creation ok\n");
	//		}
	//	}
	//}




	g_server->finalize();
}