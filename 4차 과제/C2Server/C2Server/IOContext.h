#pragma once

#include "pre_compile.h"

using namespace c2::enumeration;
struct IoContext
{
	OVERLAPPED	overalpped;
	IoType		io_type;
	void*		session;
};