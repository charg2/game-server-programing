#pragma once
#include <queue>
#include <Windows.h>
#include "../OuterServer.h"


#define do_sync(session_id, task, after_tick) local_timer->push_timer_task(session_id, task, after_tick);
#define do_time_job(session_id, task, after_tick) do_timer_job();

//using TimerTask = void(*)(uint64_t);
class OuterServer;

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
	TimeTaskScheduler() : timer_job_queue{}, current_tick {}
	{
		current_tick = GetTickCount64();
	}

	~TimeTaskScheduler()
	{
	}

	void push_timer_task(uint64_t session_id, TimerTaskType task, uint64_t after_tick, uint64_t target_id)
	{
		timer_job_queue.push({ session_id, task, after_tick + current_tick, target_id });
	}

	void bind_server(OuterServer* server)
	{
		this->server = server;
	}

	void do_timer_job()
	{
		current_tick = GetTickCount64();

		while( timer_job_queue.empty() == false )
		{
			const TimerTask& timer_job = timer_job_queue.top();
			if (current_tick < timer_job.execution_tick)
			{
				break;
			}

			server->on_timer_service(timer_job);

			timer_job_queue.pop();
		}
	}

private:	
	using TimerJobQueue = std::priority_queue<TimerTask, std::vector<TimerTask>, TimerTaskComparer>;
	TimerJobQueue	timer_job_queue;
	uint64_t		current_tick; 
	OuterServer*	server;
};

extern inline thread_local TimeTaskScheduler* local_timer {};