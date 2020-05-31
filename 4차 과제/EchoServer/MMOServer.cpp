//#include "../C2Server/C2Server/pre_compile.h"


#include "main.h"
#include "../C2Server/C2Server/OuterServer.h"
#include "MMOServer.h"
#include "Timer.h"

MMOServer::MMOServer() : OuterServer{}, zone{}
{
	zone = new MMOZone{};
}

MMOServer::~MMOServer()
{
}

void MMOServer::init_simulator()
{

}

void MMOServer::on_create_sessions(size_t capacity)
{
	session_mapping_helper(MMOSession);
}

void MMOServer::on_connect(uint64_t session_id){}
void MMOServer::on_disconnect(uint64_t session_id) 
{
	

}
bool MMOServer::on_accept(Session* session) 
{ 
	//printf( "accept() : %d \n", session->session_id );
	return true; 
}
void MMOServer::on_wake_io_thread(){}
void MMOServer::on_sleep_io_thread(){}
void MMOServer::custom_precedure(uint64_t idx) {}
void MMOServer::on_update()
{

}

#include "../C2Server/C2Server/IO/KeyManager.h"
void MMOServer::on_start()
{
	KeyManager km;

	for (;;)
	{
		if (km.key_down(VK_RETURN))
		{
			printf("hi\n");
		}

		Sleep(30);
	}

	return;
}

MMOActor* MMOServer::getActor(uint64_t session_id)
{
	return ((MMOSession*)sessions[(uint16_t)session_id])->get_actor();
}

MMOZone* MMOServer::get_zone()
{
	return zone;
}

