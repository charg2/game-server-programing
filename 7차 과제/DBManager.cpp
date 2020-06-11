#include "util/exception.h"
#include "DBHelper.h"
#include "DBManager.h"
#include "contents_enviroment.h"
#include "core/enviroment.h"
#include "MMODBTask.h"
#include "db_statement.h"

DBManager::DBManager() : db_completion_port{ INVALID_HANDLE_VALUE }, db_witer_thread { INVALID_HANDLE_VALUE }
{
	db_writing_task_queue = new MPSCQueue<DBTask*>();
	db_reader_threads = new HANDLE[c2::global::concurrent_db_reader_thread_count];
}

DBManager::~DBManager()
{
	delete db_writing_task_queue;
	delete[] db_reader_threads;
}

bool DBManager::initialize()
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

	init_db_threads();

	return true;
}

void DBManager::finalize()
{
	DbHelper::finalize();
}

void DBManager::bind_server_completion_port(HANDLE server_completion_port)
{
	c2::util::assert_if_false(0 < server_completion_port);

	this->server_completion_port = server_completion_port;
}

void DBManager::post_db_reading_task(DBTask* task)
{
	if (false == PostQueuedCompletionStatus(db_completion_port, c2::constant::DB_SIGN, task->session_id, (LPOVERLAPPED)task))
	{
		c2::util::crash_assert();
	}
}

void DBManager::post_db_writing_task(DBTask* task)
{
	db_writing_task_queue->push(task);
}

void DBManager::init_db_threads()
{
	size_t i{};
	for (; i < c2::global::concurrent_db_reader_thread_count; ++i)
	{
		db_reader_threads[i] = (HANDLE)_beginthreadex(NULL, 0, db_reader, (LPVOID)i, NULL, NULL);
		if (db_reader_threads[i] == NULL || (size_t)db_reader_threads[i] % 4 != 0)
		{
			c2::util::crash_assert();
		}
	}

	db_witer_thread = (HANDLE)_beginthreadex(NULL, 0, db_writer, (LPVOID)i, NULL, NULL);
	if (db_witer_thread == NULL || (size_t)db_witer_thread  % 4 != 0)
	{
		c2::util::crash_assert();
	}
}

void DBManager::post_task_to_server(DBTask* task)
{
	if (false == PostQueuedCompletionStatus(server_completion_port,(DWORD)c2::constant::DB_SIGN, task->session_id, (LPOVERLAPPED)task))
	{
		printf("DBManager::post_task_to_server::PostQueuedCompletionStatus failure  %d \n", WSAGetLastError() );
		c2::util::crash_assert();
	}
}


uint32_t __stdcall DBManager::db_writer(LPVOID param)
{
	c2::local::db_thread_id = reinterpret_cast<int32_t>(param);

	DBManager*	dbm = g_db_manager;
	HANDLE		server_completion_port = dbm->server_completion_port;
	DBTask*		task;

	for (;;)
	{
		while (dbm->db_writing_task_queue->try_pop(task))
		{
			switch (task->type)
			{
				case DTT_UPDATE_ACTOR_POSITION:
				{
					//UpdatePositionTask* update_task1 = new UpdatePositionTask{ { DTT_UPDATE_ACTOR_POSITION, task->session_id, task->need_result }, 0, 0, 0, };
					UpdatePositionTask* update_task = reinterpret_cast<UpdatePositionTask*>(task);
					DbHelper			db_helper;

					db_helper.bind_param_int(&update_task->user_id);
					db_helper.bind_param_int(&update_task->y);
					db_helper.bind_param_int(&update_task->x);

					if (true == db_helper.execute(sql_update_actor_position))
					{
						if (true == db_helper.fetch_row())
						{
							delete update_task;
							break;
						}
					}
					
					wprintf(L"actor position update failure...\n");
					break;
				}
				case DTT_CREATE_ACTOR:
				{
					//CreateActorTask* update_task1 = new CreateActorTask{ { DTT_UPDATE_ACTOR_POSITION, task->session_id, task->need_result }, 0, 0, 0, };
					CreateActorTask*	create_task = reinterpret_cast<CreateActorTask*>(task);
					DbHelper			db_helper;

					db_helper.bind_param_str(create_task->name);
					if (true == db_helper.execute(sql_create_actor))
					{
						if (true == db_helper.fetch_row())
						{
							// 성공인지 실패인지 결과를 알려줘야 함.
							dbm->post_task_to_server(task);
						}
						// db 오류는 알려주고...
					}

					break;
				}
				default:
					break;
			}
		}

		Sleep(1);
	}

	return 1;
}


uint32_t __stdcall DBManager::db_reader(LPVOID param)
{
	c2::local::	db_thread_id			= reinterpret_cast<int32_t>(param);
	DBManager*	db_manager				= g_db_manager;
	HANDLE		db_completion_port		= g_db_manager->db_completion_port;
	HANDLE		server_completion_port	= db_manager->server_completion_port;

	for (;;)
	{
		DWORD			transfered_bytes	{};
		LPOVERLAPPED	overlapped_ptr		{};
		ULONG_PTR		completion_key		{};

		bool ret = GetQueuedCompletionStatus(db_completion_port, &transfered_bytes, &completion_key, &overlapped_ptr, INFINITE);
		if (overlapped_ptr == nullptr && transfered_bytes == 0   && completion_key == 0)
		{
			break;
		}
		DBTask* db_task = reinterpret_cast<DBTask*>(overlapped_ptr);
		switch (db_task->type)
		{
			case DBTaskType::LOAD_ACTOR:
			{	
				LoadActorTask*	load_actor_db_task = reinterpret_cast<LoadActorTask*>(db_task);
				DbHelper		db_helper;
				
				int id, level, hp, exp, y, x;

				db_helper.bind_param_str(load_actor_db_task->name);

				db_helper.bind_result_column_int(&load_actor_db_task->user_id);
				db_helper.bind_result_column_str(load_actor_db_task->name, count_of(load_actor_db_task->name));
				db_helper.bind_result_column_int(&load_actor_db_task->y);
				db_helper.bind_result_column_int(&load_actor_db_task->x);
				db_helper.bind_result_column_int(&load_actor_db_task->level);
				db_helper.bind_result_column_int(&load_actor_db_task->exp);
				db_helper.bind_result_column_int(&load_actor_db_task->hp);
				if (db_helper.execute(sql_load_actor))
				{
					load_actor_db_task->is_success = db_helper.fetch_row();
				
					db_manager->post_task_to_server(load_actor_db_task);
				}

				break;
			}
			default:
				break;
		}
	}

	return 1;
}
