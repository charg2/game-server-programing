#pragma once


namespace c2
{
	namespace constant
	{
		constexpr inline size_t SEND_SIGN = 5001U;
		constexpr inline size_t MAX_CONCURRENT_SEND_COUNT = 256U;
		constexpr inline size_t CACHE_LINE = 64U;
	}

	namespace enumeration
	{
		enum IoType 
		{
			IO_ACCEPT,
			IO_RECV,
			IO_SEND,
			IO_DISCONNECT,
		};

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
		{
		};

		enum ThreadType
		{
			TT_ACCEPTER,
			TT_IO,
			TT_CUSTOM,
			TT_MAX
		};
	}

	namespace global
	{
	}
}


