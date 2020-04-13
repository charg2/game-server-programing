#pragma once
#include <Windows.h>
//#include "Session.h"
//struct Session;

struct OverlappedContext
{
	OVERLAPPED	overalpped;
	void*		session;
};