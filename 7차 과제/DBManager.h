//#pragma once
//
//#include <cstdint>
//#include "DbTask.h"
//#include "concurrency/MPSCQueue.h"
//
//// job에 포함된게 task.
//// TASK < JOB 
////
//
//
//
//
//using namespace c2::concurrency;
//class DBManager
//{
//public:
//	DBManager();
//	~DBManager();
//
//	bool initialize();
//	void finalize();
//
//
//	void bind_server_completion_port(HANDLE server_completion_port);
//	void post_db_reading_task(DBTask* task); // for procedure
//	void post_db_writing_task(DBTask* task); // for procedure
//
//private:
//	void init_db_threads();
//
//	void post_task_to_server(DBTask* task);
//
//	static uint32_t WINAPI 	db_writer(LPVOID param);
//	static uint32_t WINAPI 	db_reader(LPVOID param);
//
//private:
//	MPSCQueue<DBTask*>*	db_writing_task_queue; /// 이걸 쓸지 저걸 쓸지 고민중.
//
//	HANDLE				db_witer_thread;
//	HANDLE*				db_reader_threads;
//	HANDLE				db_completion_port;
//	HANDLE				server_completion_port;
//};
//
//extern inline DBManager* g_db_manager{};