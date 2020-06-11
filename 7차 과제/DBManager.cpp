#include "util/exception.h"
#include "DBHelper.h"
#include "DBManager.h"
#include "contents_enviroment.h"
#include "core/enviroment.h"

DbManager::DbManager() : db_completion_port { INVALID_HANDLE_VALUE}
{
	db_writing_task_queue = new MPSCQueue<DbTask*>();
}

DbManager::~DbManager()
{
	delete db_writing_task_queue;
}

bool DbManager::initialize()
{
	db_completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, c2::global::concurrent_db_reader_thread_count);
	if (NULL == db_completion_port || db_completion_port == INVALID_HANDLE_VALUE)
	{
		//debug_code(printf("%s::%s \n", __FILE__, __LINE__));
		//this->custom_last_os_error = c2::enumeration::ER_COMPLETION_PORT_INITIATION_FAILURE;
		return false;
	}


	if (false == DbHelper::initialize(c2::global::db_connection_string, c2::global::concurrent_db_reader_thread_count))
	{
		return false;
	}


	return true;
}

void DbManager::finalize()
{
}

void DbManager::bind_server_completion_port(HANDLE server_completion_port)
{
	c2::util::assert_if_false(0 < server_completion_port);

	this->server_completion_port = server_completion_port;
}

void DbManager::post_db_reading_task(DbTask* task)
{
	bool ret =PostQueuedCompletionStatus(db_completion_port, 0, 0, 0);
	if (ret == 1)
	{}
}

void DbManager::post_db_writing_task(DbTask* task)
{
	db_writing_task_queue->push(task);
}

void DbManager::post_task_to_server(DbTask* task)
{
	if (false == PostQueuedCompletionStatus(server_completion_port, (DWORD)task, task->session_id, (LPOVERLAPPED)c2::constant::DB_SIGN))
	{
		c2::util::crash_assert();
	}
}


uint32_t __stdcall DbManager::db_writer(LPVOID param)
{
	DbManager*	dbm = g_db_manager;
	HANDLE		server_completion_port = dbm->server_completion_port;
	DbTask*		tsk;

	for (;;)
	{
		while (dbm->db_writing_task_queue->try_pop(tsk))
		{
			switch (tsk->type)
			{
			case DTT_UPDATE_ACTOR_POSITION:
			{
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
				break;
			}
			case DTT_CREATE_ACTOR:
			{
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
				break;
			}
			default:
				break;
			}

			
			// 처리 결과를 넘겨야 하는 작업은 db 서버로 넘긴다. 
			if (true == tsk->need_result)
			{
				// dbm->post_task_to_server(tsk);
			}
		}

		Sleep(1);
	}

	return 1;
}


uint32_t __stdcall DbManager::db_reader(LPVOID param)
{
	c2::local::	db_thread_id			= reinterpret_cast<int32_t>(param);
	DbManager*	db_manager				= g_db_manager;
	HANDLE		db_completion_port		= g_db_manager;
	HANDLE		server_completion_port	= db_manager->server_completion_port;

	for (;;)
	{
		DWORD			transfered_bytes	{};
		LPOVERLAPPED	overlapped_ptr		{};
		ULONG_PTR		completion_key		{};

		bool ret = GetQueuedCompletionStatus(db_completion_port, &transfered_bytes, &completion_key, &overlapped_ptr, INFINITE);
		if (transfered_bytes == 0 && overlapped_ptr == nullptr && completion_key == 0)
		{
			break;
		}

		DbTask* db_task = reinterpret_cast<DbTask*>(overlapped_ptr);
		
		switch (db_task->type)
		{
			case DbTaskType::LOAD_ACTOR:
			{	
				DbTask*		db_task = reinterpret_cast<DbTask*>(overlapped_ptr);
				DbHelper	db_helper;
				
				//	int id, level, hp, exp, y, x;
				//	char_t name[50];
				//	wchar_t password[100];

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
				//			wprintf(L"%d %d %d %d %d, %d| \n", id,  level, y, x, hp, exp);
				//		}
				//	}

				break;
			}
			default:
				break;
		}


		// 처리 결과를 넘겨야 하는 작업은 db 서버로 넘긴다. 
		if (true == db_task->need_result)
		{
			// dbm->post_task_to_server(tsk);
		}
	}

	return 1;
}
