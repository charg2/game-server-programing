#include "main.h"
#include "EchoServer.h"

void main()
{
	EchoServer server;
	
	server.load_config_using_json(L"config.json");

	server.initialize();

	
	Sleep(INFINITE);

	server.finalize();
}