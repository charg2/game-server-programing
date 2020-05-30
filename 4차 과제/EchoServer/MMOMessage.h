#pragma once

#include <cstdint>
#include "../C2Server/C2Server/pre_compile.h"

struct MMOMessage
{
	c2::Packet* lonin_ok_packet;
	uint64_t	session_id;
	MMOSession* session;
	char		type;

};