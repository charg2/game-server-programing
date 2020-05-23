#include "main.h"
#include "MMOServer.h"

void main()
{
	MMOServer server;
	
	server.setup_dump();

	server.load_config_using_json(L"config.json");

	server.initialize();

	server.start();
	
	server.finalize();
}