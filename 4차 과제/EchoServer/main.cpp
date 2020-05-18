#include "main.h"
#include "EchoServer.h"


LONG CALLBACK UnhandledExceptionHandler(EXCEPTION_POINTERS* lpExceptionInfo)
{
	printf("Exception : 0x%08X\r\n",
		lpExceptionInfo->ExceptionRecord->ExceptionCode);
	printf("Exception Address : 0x%08p\r\n",
		lpExceptionInfo->ExceptionRecord->ExceptionAddress);

	return EXCEPTION_EXECUTE_HANDLER;
}


void main()
{
	EchoServer server;
	
	SetUnhandledExceptionFilter(c2::diagnostics::ExceptionFilter);
	//SetUnhandledExceptionFilter(UnhandledExceptionHandler);

	server.load_config_using_json(L"config.json");

	server.initialize();

	server.start();
	
	server.finalize();
}