#pragma once
#include <Windows.h>


class CompletionPort
{
	CompletionPort(size_t concurrent_thread_cnt = 0);
	~CompletionPort();

	bool register_device(HANDLE device, size_t unique_id);
	bool take();
	bool try_take();
	bool push();
	const HANDLE get_native_handle() const;

private:
	HANDLE completion_port;
};

