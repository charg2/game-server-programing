#pragma once

#include "mmo_function.hpp"

namespace c2
{
	namespace constant
	{
		/// core 
		constexpr inline size_t SEND_SIGN = 5001U;
		//constexpr inline size_t SEND_SIGN = 'SEND' + 1;
		//static_assert( ( SEND_SIGN % 4 ) != 0, "Send sign must  " );
		constexpr inline size_t ASYNC_ACCEPT_SIGN = 10003U; // 유저수에 따라 조정해줘야 함.
		constexpr inline size_t MAX_CONCURRENT_SEND_COUNT = 256U;
		constexpr inline size_t CACHE_LINE = 64U;
		constexpr inline size_t GQCS_TIME_OUT = 20;

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

	}

	namespace global
	{
	}
}


