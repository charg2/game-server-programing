//#include "../C2Server/C2Server/pre_compile.h"
//#include "EchoServer.h"

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
