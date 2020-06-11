#pragma once

#include "mmo_function.hpp"

namespace c2
{
	namespace constant
	{
		constexpr unsigned char D_UP = 0;
		constexpr unsigned char D_DOWN = 1;
		constexpr unsigned char D_LEFT = 2;
		constexpr unsigned char D_RIGHT = 3;

		constexpr int MAX_ID_LEN = 50;
		constexpr int MAX_STR_LEN = 255;
		constexpr int MAX_CHAT_LEN = 50;

		constexpr int FOV_WIDTH = 17;
		constexpr int FOV_HEIGHT = 17;

		constexpr int SECTOR_WIDTH = FOV_WIDTH * 2;
		constexpr int SECTOR_HEIGHT = FOV_HEIGHT * 2;

		constexpr int FOV_HALF_WIDTH = FOV_WIDTH / 2;
		constexpr int FOV_HALF_HEIGHT = FOV_HEIGHT / 2;

		constexpr int BROADCAST_HEIGHT = 11;
		constexpr int BROADCAST_WIDTH = 11;
		constexpr int MAP_WIDTH = 800;
		constexpr int MAP_HEIGHT = 800;

		constexpr int NPC_ID_OFFSET = 10'000;
		constexpr size_t MAP_NAX_HEIGHT_INDEX = calc_static_map_index<MAP_HEIGHT, SECTOR_HEIGHT>();
		constexpr size_t MAP_NAX_WIDTH_INDEX = calc_static_map_index<MAP_WIDTH, SECTOR_WIDTH>();
		constexpr int INITIAL_HP = 200;
		constexpr int LEVEL_MAXIMUM_EXP = 200;  // level * LEVLE_MAXIMUM_EXPÇÏ¸é µÊ.
	}


	namespace enumeration
	{
		enum MMOActorState : unsigned long long
		{
			ACS_NONE,
			ACS_BEFORE_LOGIN,
			ACS_AFTER_LOGIN,
			ACS_PLAYING,
		};
	}

	namespace global
	{
		extern inline wchar_t	db_connection_string[128]			{ };
		extern inline int32_t	db_read_thread_count				{ - 1};
		extern inline uint32_t	max_npc								{ };
		extern inline int32_t	concurrent_db_reader_thread_count	{ -1 };
	}

	namespace local
	{
		extern inline thread_local int32_t	db_thread_id	{ -1 };
	}
}


