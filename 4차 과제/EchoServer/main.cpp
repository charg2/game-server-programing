#include "main.h"
#include "EchoServer.h"

void main()
{
	EchoServer server;
	
	server.setup_dump();

	server.load_config_using_json(L"config.json");

	server.initialize();

	server.start();
	
	server.finalize();
}