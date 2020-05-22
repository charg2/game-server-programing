#include "main.h"

void main()
{
	MMOServer* server = new MMOServer();
	
	server->setup_dump();

	server->load_config_using_json(L"config.json");

	server->initialize();

	server->start();
	
	server->finalize();
}

