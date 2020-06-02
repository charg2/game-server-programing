#pragma once
#include <queue>
#include <Windows.h>
#include "../OuterServer.h"


#define do_sync(session_id, task, after_tick) local_timer->push_timer_task(session_id, task, after_tick);
#define do_time_job(session_id, task, after_tick) do_timer_job();

//using TimerTask = void(*)(uint64_t);
class OuterServer;



enum TimerTaskType
{
	TTT_MOVE_NPC,

};

struct TimerTask
{
	uint64_t		server_id;
	TimerTaskType	task_type;
	uint64_t		execution_tick;
	uint64_t		target_id;
};


struct TimerTaskComparer
{
	bool operator()(const TimerTask& lhs, const TimerTask& rhs)
	{
		return lhs.execution_tick > rhs.execution_tick;
	}
};



class TimeTaskScheduler
{
public:
	TimeTaskScheduler();
	~TimeTaskScheduler();
	void push_timer_task(uint64_t session_id, TimerTaskType task, uint64_t after_tick, uint64_t target_id);
	void bind_server(OuterServer* server);
	void do_timer_job();

private:
	using TimerJobQueue = std::priority_queue<TimerTask, std::vector<TimerTask>, TimerTaskComparer>;
	TimerJobQueue	timer_job_queue;
	uint64_t		current_tick;
	OuterServer* server;
};

extern inline thread_local TimeTaskScheduler* local_timer{};