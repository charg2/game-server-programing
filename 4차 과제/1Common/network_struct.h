#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct OtherPlayerBinary
{
	uint64_t	session_id;
	wchar_t		nickname[16];
	int8_t		x, y, z;
};
#pragma pack(pop)