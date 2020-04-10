#ifndef DUMP_H
#define DUMP_H

#include <Windows.h>
#include <DbgHelp.h>
#include <thread>
#pragma comment (lib, "Dbghelp")

LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* exceptionInfo);

#endif