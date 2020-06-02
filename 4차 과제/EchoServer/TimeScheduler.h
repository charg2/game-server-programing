#pragma once
#include <queue>
#include <Windows.h>


using TimerTask = void(*)(uint64_t);

struct TimerJobElement
{
	TimerJobElement(uint64_t session_id, TimerTask task, uint64_t execution_tick) = delete;
	//~TimerJobElement()
	//{}

	uint64_t	session_id;
	TimerTask	task;
	uint64_t	execution_tick;
};


struct TimerJobComparer
{
	bool operator()(const TimerJobElement& lhs, const TimerJobElement& rhs)
	{
		return lhs.execution_tick > rhs.execution_tick;
	}
};



class Timer
{
	Timer() : timer_job_queue{}, current_tick {}
	{
		current_tick = GetTickCount64();
	}

	~Timer()
	{
	}

	void push_timer_job(uint64_t session_id, TimerTask task, uint64_t after_tick)
	{
		timer_job_queue.push({ session_id, task, after_tick + current_tick });
	}


	void do_timer_job()
	{
		current_tick = GetTickCount64();

		while( timer_job_queue.empty() == false )
		{
			const TimerJobElement& timer_job = timer_job_queue.top();

			if (current_tick < timer_job.execution_tick)
			{
				break;
			}

			timer_job.task(timer_job.session_id);
		}
	}

private:
	using TimerJobQueue = std::priority_queue<TimerJobElement, std::vector<TimerJobElement>, TimerJobComparer>;
	TimerJobQueue	timer_job_queue;
	uint64_t		current_tick;
};

