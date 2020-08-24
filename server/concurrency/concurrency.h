#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#define Out 

namespace c2::concurrency
{
#ifdef _WIN64
	constexpr size_t kDeadBeef				{ 0xDDEEAADDBBEEEEFF };
	constexpr size_t kDeadDead				{ 0xDEADDEADDEADDEAD };
#else
	constexpr size_t kDeadBeef				{ 0xDEADBEEF };
	constexpr size_t kDeadDead				{ 0xDEADDEAD };
#endif
}


