#pragma once

#include <cstdint>
#include "concurrency/MPSCQueue.h"

// job에 포함된게 task.
// TASK < JOB 
//

enum DbTaskType
{
	LOAD_ACTOR,
	DTT_UPDATE_ACTOR_POSITION = 0,

	DTT_CREATE_ACTOR, 
}; 

struct DbTask
{
	DbTaskType	type;
	uint64_t	session_id;
};

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