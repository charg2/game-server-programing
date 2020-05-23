#include "main.h"
#include "EchoServer.h"

void main()
{
	EchoServer server;
	
	SetUnhandledExceptionFilter(c2::diagnostics::exception_filter);
	//SetUnhandledExceptionFilter(UnhandledExceptionHandler);

	server.load_config_using_json(L"config.json");

	server.initialize();

	server.start();
	
	server.finalize();
}