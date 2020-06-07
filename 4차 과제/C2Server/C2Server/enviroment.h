#pragma once

#include "../../EchoServer/mmo_function.hpp"

namespace c2
{
	namespace constant
	{
		/// core 
		constexpr inline size_t SEND_SIGN = 5001U;
		//constexpr inline size_t SEND_SIGN = 'SEND' + 1;
		//static_assert( ( SEND_SIGN % 4 ) != 0, "Send sign must  " );
		constexpr inline size_t ASYNC_ACCEPT_SIGN = 10003U; // �������� ���� ��������� ��.
		constexpr inline size_t MAX_CONCURRENT_SEND_COUNT = 256U;
		constexpr inline size_t CACHE_LINE = 64U;
		constexpr inline size_t GQCS_TIME_OUT = 20;

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

		constexpr int MAX_NPC = 200'000;
		//constexpr int MAX_NPC = 20'000;
		constexpr int NPC_ID_OFFSET = 10'000;
		constexpr size_t MAP_NAX_HEIGHT_INDEX = calc_static_map_index<MAP_HEIGHT, SECTOR_HEIGHT>();
		constexpr size_t MAP_NAX_WIDTH_INDEX = calc_static_map_index<MAP_WIDTH, SECTOR_WIDTH>();
		constexpr int INITIAL_HP = 200;
		constexpr int LEVEL_MAXIMUM_EXP = 200;  // level * LEVLE_MAXIMUM_EXP�ϸ� ��.
	}


	namespace enumeration
	{
		enum ErrorCode
		{
			ER_NONE,

			ER_WINSOCK_LIB_FAILURE = 100,
			ER_COMPLETION_PORT_INITIATION_FAILURE,

			ER_WSA_LIB_INIT_FAILURE = 200,
			ER_ACCEPTEX_LAODING_FAILURE,
			ER_DISCONNECTEX_LAODING_FAILURE,
			ER_CONNECTEX_LAODING_FAILURE,

			ER_ASSOCIATIVE_COMPLETION_PORT_FALIURE,

			ER_LISTEN_FIALURE,
		};

		enum DisconnectReason
		{
			DR_NONE,
			DR_WSA_RECV_FAILURE,
			DR_WSA_SEND_FAILURE,
			DR_RECV_BUFFER_FULL,
		};

		enum SessionState
		{};


		enum ThreadType
		{
			TT_ACCEPTER,
			TT_IO,
			TT_IO_AND_TIMER,
			TT_CUSTOM,
			TT_MAX
		};



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
	}
}


