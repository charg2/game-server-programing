#include "dump.h"

#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <signal.h>

#include "Exception.h"

#define MAX_BUFF_SIZE 1024


void myPurecallHandler()
{
	c2::util::assert_if_false(false);
}
void myInvalidParamaterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* filfile, unsigned int line, uintptr_t pReserved)
{
	c2::util::assert_if_false(false);
}

void signalHandler(int error)
{
	c2::util::assert_if_false(false);
}

LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* e)
{
	static uint64_t dump_count = 0;
	uint64_t ret = InterlockedIncrement(&dump_count);

	_invalid_parameter_handler oldHandler, newHandler;
	newHandler = myInvalidParamaterHandler;

	oldHandler = _set_invalid_parameter_handler(newHandler);	// crt 함수에 null 포인터 등을 넣었을 때....
	_CrtSetReportFile(_CRT_WARN, 0);							// CRT 오류 메시지 표시 중단. 바로 덤프로 남도록.
	_CrtSetReportFile(_CRT_ASSERT, 0);							// CRT 오류 메시지 표시 중단. 바로 덤프로 남도록.
	_CrtSetReportFile(_CRT_ERROR, 0);							// CRT 오류 메시지 표시 중단. 바로 덤프로 남도록.

	//_CrtSetReportHook(_custom_Report_hook);

	//---------------------------------------------------------------------------
	// pure virtual function called 에러 핸드러를 사용자 정의 함수로 우회시킨다.
	//---------------------------------------------------------------------------
	_set_purecall_handler(myPurecallHandler);
	
	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT );

	signal(SIGABRT, signalHandler);
	signal(SIGINT, signalHandler);
	signal(SIGILL, signalHandler);
	signal(SIGFPE, signalHandler);
	signal(SIGSEGV, signalHandler);
	signal(SIGTERM, signalHandler);



	TCHAR tszFileName[MAX_BUFF_SIZE] = { 0 };
	SYSTEMTIME stTime = { 0 };
	GetSystemTime(&stTime);
	StringCbPrintf(tszFileName,
		_countof(tszFileName),
		_T("%s_%4d%02d%02d_%02d%02d%02d_%d.dmp"),
		_T("Dump"),
		stTime.wYear,
		stTime.wMonth,
		stTime.wDay,
		stTime.wHour,
		stTime.wMinute,
		stTime.wSecond,
		ret);



	HANDLE hFile = CreateFile(tszFileName, GENERIC_WRITE, /*FILE_SHARE_READ*/ FILE_SHARE_READ , 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Dump Error : %d \n", GetLastError());
		return EXCEPTION_EXECUTE_HANDLER;
	}
	MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
	exceptionInfo.ThreadId = GetCurrentThreadId();
	exceptionInfo.ExceptionPointers = e;
	exceptionInfo.ClientPointers = FALSE;

	MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory |
		MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithFullMemoryInfo |
		MiniDumpScanMemory | MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules | MiniDumpWithFullMemory);

	// MiniDumpWriteDump를 사용하여 hFile에 덤프 기록
	MiniDumpWriteDump(GetCurrentProcess()
		, GetCurrentProcessId()
		, hFile
		, mdt//(MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory | MiniDumpScanMemory | MiniDumpWithFullMemory)
		, (e != nullptr) ? &exceptionInfo : 0
		, 0
		, NULL);

	if (hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}

	return EXCEPTION_EXECUTE_HANDLER;
}


