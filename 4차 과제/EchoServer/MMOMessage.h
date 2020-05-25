#pragma once

#include <cstdint>
#include "../C2Server/C2Server/pre_compile.h"

struct MMOMessage
{
	c2::Packet* in_packet;
	uint64_t	session_id;
	MMOSession* session;
	char		type;

};