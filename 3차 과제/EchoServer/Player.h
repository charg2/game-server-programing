#pragma once

#include <cstdint>

typedef struct Player
{
	constexpr static int8_t field_view{ 10 };

	wchar_t nickname[16];
	size_t	session_id;
	int8_t	x;
	int8_t	y;

	bool is_nearby(const Player* other);
};

