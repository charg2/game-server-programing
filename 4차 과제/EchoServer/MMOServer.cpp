//#include "../C2Server/C2Server/pre_compile.h"


#include "main.h"
#include "../C2Server/C2Server/OuterServer.h"
#include "MMOServer.h"

EchoServer::EchoServer() : OuterServer{}
{
}

EchoServer::~EchoServer()
{
}

void EchoServer::on_create_sessions(size_t capacity)
{
	//EchoSession* EchoSession_ptr = (EchoSession*)HeapAlloc(session_heap, 0, sizeof(EchoSession) * capacity); 
	//for (size_t i = 0; i < capacity; ++i) 
	//{ 
	//	sessions[i] = (Session*)&EchoSession_ptr[i]; 
	//	new(sessions[i]) EchoSession(); 
	//} 

	session_mapping_helper(EchoSession);
}

void EchoServer::on_connect(uint64_t session_id){}
void EchoServer::on_disconnect(uint64_t session_id) { }
bool EchoServer::on_accept(Session* session) { return true; }
void EchoServer::on_wake_io_thread(){}
void EchoServer::on_sleep_io_thread(){}
void EchoServer::custom_precedure(uint64_t idx) {}
void EchoServer::on_update()
{
	static size_t count = 0;

	count += 1;
	if (count > 100)
	{
		printf("total send count : %lld \n total recv count : %lld \n total send bute : %lld \n total recv byte : %lld \n "\
			, total_sent_count
			, total_recv_count
			, total_sent_bytes
			, total_recv_bytes );

		count = 0;
	}
}

//void EchoServer::on_update()
//{
//	//
//}
