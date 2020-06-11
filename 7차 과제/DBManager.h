#pragma once

#include <cstdint>
#include "DbTask.h"
#include "concurrency/MPSCQueue.h"

// job에 포함된게 task.
// TASK < JOB 
//




using namespace c2::concurrency;
class DbManager
{
public:
	DbManager();
	~DbManager();

	bool initialize();
	void finalize();

	void bind_server_completion_port(HANDLE server_completion_port);
	void post_db_reading_task(DbTask* task); // for procedure
	void post_db_writing_task(DbTask* task); // for procedure

private:
	void post_task_to_server(DbTask* task);

	static uint32_t WINAPI 	db_writer(LPVOID param);
	static uint32_t WINAPI 	db_reader(LPVOID param);

private:
	MPSCQueue<DbTask*>*	db_writing_task_queue; /// 이걸 쓸지 저걸 쓸지 고민중.

	HANDLE				db_thread;
	HANDLE				db_completion_port;
	HANDLE				server_completion_port;
};

extern inline DbManager* g_db_manager{};