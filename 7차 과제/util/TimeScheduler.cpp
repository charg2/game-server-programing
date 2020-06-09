#include "../pre_compile.h"
#include "TimeScheduler.h"

TimeTaskScheduler::TimeTaskScheduler()
	: timer_job_queue{}, current_tick{}
{
	current_tick = GetTickCount64();
}
TimeTaskScheduler::~TimeTaskScheduler()
{
}

void TimeTaskScheduler::push_timer_task(uint64_t session_id, TimerTaskType task, uint64_t after_tick, uint64_t target_id)
{
	timer_job_queue.push({ after_tick + current_tick, session_id, task, target_id });
}

void TimeTaskScheduler::bind_server(OuterServer* server)
{
	this->server = server;

}

void TimeTaskScheduler::do_timer_job()
{
	current_tick = GetTickCount64();

	while (timer_job_queue.empty() == false)
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
