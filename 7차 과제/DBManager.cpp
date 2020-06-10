#include "DBHelper.h"
#include "DBManager.h"
#include "contents_enviroment.h"

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

}

void DbManager::post_db_reading_task(DbTask* task)
{
	// rr
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
	if (false == PostQueuedCompletionStatus(server_completion_port, 0, 0, nullptr))
	{
		// GetLastError();
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
				break;
			case DTT_CREATE_ACTOR:
				break;

			default:
				break;
			}

			
			// 처리 결과를 넘겨야 하는 작업은 db 서버로 넘긴다. 
			// dbm->post_task_to_server(tsk);
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

		switch ((size_t)overlapped_ptr)
		{
		case DbTaskType::LOAD_ACTOR:
			break;

		default:
			break;
		}
	}

	return 1;
}
