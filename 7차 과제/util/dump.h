#ifndef DUMP_H
#define DUMP_H

#include <Windows.h>
#include <DbgHelp.h>
#include <thread>
#pragma comment (lib, "Dbghelp")

namespace c2::diagnostics
{
	LONG WINAPI exception_filter(EXCEPTION_POINTERS* exceptionInfo);
}
#endif