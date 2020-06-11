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
#include "DBManager.h"
#include "db_statement.h"
void main()
{
	setlocale(LC_ALL, "");

	g_server		= new MMOServer();

	g_server->setup_dump();

	g_server->load_config_using_json(L"config.json");
	
	g_db_manager = new DBManager();

	if( false == g_db_manager->initialize() )
		return; 

	g_server->init_npcs();

	g_server->initialize();

	g_db_manager->bind_server_completion_port(g_server->get_completion_port());

	g_server->start();

	g_server->finalize();

	//if (false == DbHelper::initialize(global::db_connection_string, global::concurrent_db_reader_thread_count))
	//	return;


	//c2::local::db_thread_id = 0;

	//{
	//	DbHelper db_helper;
	//	int id, level, hp, exp, y, x;
	//	char name[50];

	//	int uid = 100;
	//	wchar_t password[100];
	//	db_helper.bind_param_str("actor_1");

	//	db_helper.bind_result_column_int(&id);
	//	db_helper.bind_result_column_str(name, count_of(name));
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
	//	char name[100];

	//	int uid = 101;
	//	db_helper.bind_param_str("actor_5");

	//	db_helper.bind_result_column_int(&id);
	//	db_helper.bind_result_column_str(name, count_of(name));
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
	//	const char* name{ "actor_21" };
	//	wchar_t password[100];

	//	db_helper.bind_param_str(name);
	//	if (true == db_helper.execute(sql_create_actor))
	//	{
	//		if (true == db_helper.fetch_row())
	//		{
	//			wprintf(L"actor creation ok\n");
	//		}
	//	}
	//}




}