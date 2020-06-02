#include "tls.h"

extern thread_local void*	thread_local_storage[MAX_TLS_INDEX]	{ };
extern thread_local int64_t current_tls_idx						{ -1 };