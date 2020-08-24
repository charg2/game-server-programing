#pragma once

#include <cstdint>
#include "concurrency.h"

#define get_tls_idx()		InterlockedIncrement64(&current_tls_idx)
#define get_tls(index)		thread_local_storage[index]
#define reset_tls(index)	thread_local_storage[index] = nullptr;
#define set_tls(index, ptr) thread_local_storage[index] = ptr;

constexpr size_t				MAX_TLS_INDEX = 16;

extern inline thread_local int64_t		current_tls_idx{ -1 };
extern inline thread_local void*		thread_local_storage[MAX_TLS_INDEX]{};