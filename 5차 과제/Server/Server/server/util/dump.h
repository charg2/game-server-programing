#pragma once

#include <Windows.h>
#include <DbgHelp.h>
#include <thread>
#pragma comment (lib, "Dbghelp")


LONG WINAPI exception_filter(EXCEPTION_POINTERS* exceptionInfo);
