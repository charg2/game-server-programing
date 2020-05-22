//#include "../C2Server/C2Server/pre_compile.h"


#include "main.h"
#include "../C2Server/C2Server/OuterServer.h"
#include "MMOServer.h"

MMOServer::MMOServer() : OuterServer{}
{
}

MMOServer::~MMOServer()
{
}

void MMOServer::on_create_sessions(size_t capacity)
{
	MMOSession* EchoSession_ptr = (MMOSession*)HeapAlloc(session_heap, HEAP_GENERATE_EXCEPTIONS, sizeof(MMOSession) * capacity);

	for (size_t i = 0; i < capacity; ++i) 
	{ 
		sessions[i] = &EchoSession_ptr[i]; 
		printf("%d : %p \n", i, &EchoSession_ptr[i]);
		new(sessions[i]) MMOSession();
	} 
	//session_mapping_helper(MMOSession);
}

void MMOServer::on_connect(uint64_t session_id){}
void MMOServer::on_disconnect(uint64_t session_id) { }
bool MMOServer::on_accept(Session* session) { return true; }
void MMOServer::on_wake_io_thread(){}
void MMOServer::on_sleep_io_thread(){}
void MMOServer::custom_precedure(uint64_t idx) {} 
void MMOServer::on_update(){}

void MMOServer::start()
{
	while (running)
	{
		//simulator.dispatch_netwrok();
		//simulator.update();
	}
}


