#pragma once

#include <queue>
#include <Windows.h>

class OuterServer;
enum TimerTaskType
{
	TTT_ON_WAKE_FOR_NPC,

	TTT_UPDATE_FOR_NPC,
	TTT_DO_PATH_FINDING_FOR_NPC,
	TTT_ON_SLEEP_FOR_NPC,

	TTT_USER_RECOVER_HP,
	TTT_RESPAWN_FOR_NPC,
	TTT_RESPAWN_FOR_PLAYER

};

struct TimerTask
{
	uint64_t		execution_tick;
	uint64_t		actor_id; 
	TimerTaskType	task_type;
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
	using TimerTaskQueue = std::priority_queue<TimerTask, std::vector<TimerTask>, TimerTaskComparer>;
	TimerTaskQueue	timer_job_queue;
	uint64_t		current_tick; 
	OuterServer*	server;
};

extern inline thread_local TimeTaskScheduler* local_timer {};