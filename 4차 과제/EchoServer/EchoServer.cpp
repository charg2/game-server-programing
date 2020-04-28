//#include "../C2Server/C2Server/pre_compile.h"


#include "main.h"
#include "../C2Server/C2Server/OuterServer.h"
#include "EchoServer.h"

EchoServer::EchoServer()
{
}

EchoServer::~EchoServer()
{
}

void EchoServer::on_create_sessions(size_t n)
{
	EchoSession* sessions_ptr = (EchoSession*)HeapAlloc(session_heap, 0, sizeof(EchoSession) * n);
	for (size_t i = 0; i < n; ++i)
	{
		sessions[n] = sessions_ptr;
		new(sessions[n]) EchoSession();
	}
}

void EchoServer::on_connect(uint64_t session_id)
{
}

void EchoServer::on_disconnect(uint64_t session_id)
{
}

bool EchoServer::on_accept(Session* session)
{
	return true;
}

void EchoServer::on_wake_io_thread()
{
}

void EchoServer::on_sleep_io_thread()
{
}

void EchoServer::custom_precedure(uint64_t idx)
{
}
