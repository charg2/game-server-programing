//#include "../C2Server/C2Server/pre_compile.h"


#include "main.h"
#include "../C2Server/C2Server/OuterServer.h"
#include "EchoServer.h"

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
