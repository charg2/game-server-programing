#pragma once


namespace c2
{
	namespace constant
	{
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

			ER_WSA_INIT_FAILURE = 100, 
			ER_ACCEPTEX_LAODING_FAILURE,
			ER_DISCONNECTEX_LAODING_FAILURE,
			ER_CONNECTEX_LAODING_FAILURE

		};

		enum DisconnectReason
		{
			DR_NONE,
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
