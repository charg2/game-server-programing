#include "EchoServer.h"
#include "../Common/dump.h"
int main()
{
	SetUnhandledExceptionFilter(ExceptionFilter);
	
	EchoServer server;

	if (false == server.initialize())
		printf( "¤Ð¤Ð" );

	server.start();

	server.finalize();

}