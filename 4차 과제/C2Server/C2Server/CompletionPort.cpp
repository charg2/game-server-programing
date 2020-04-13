#include "CompletionPort.h"

CompletionPort::CompletionPort(size_t concurrent_thread_cnt) : completion_port{ INVALID_HANDLE_VALUE }
{
	CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, concurrent_thread_cnt);
}

CompletionPort::~CompletionPort()
{
	CloseHandle(completion_port);
}

bool CompletionPort::register_device(HANDLE device, size_t id)
{
	return CreateIoCompletionPort(this->completion_port, device, id, 0) == this->completion_port;
}

bool CompletionPort::take()
{
	return true;
}

bool CompletionPort::try_take()
{
	return false;
}

bool CompletionPort::push()
{
	return false;
}

const HANDLE CompletionPort::get_native_handle() const
{
	return completion_port;
}

