#pragma once


namespace c2
{
	namespace constant
	{
		constexpr size_t SEND_SIGN = 5001U; 
		constexpr size_t MAX_CONCURRENT_SEND_COUNT = 256U;
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

			ER_LISTEN_FIALURE,
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


void Session::send_packet(c2::Packet* out_packet)
{
	// packet이 들어옴.
	// concurrent_queue에 넣음.
	send_buffer.push(out_packet);

	// send중이 아니라면....
	if (this->send_flag == 0)
	{
		PostQueuedCompletionStatus(server->completion_port, 0, (ULONG_PTR)this, (LPOVERLAPPED)c2::constant::SEND_SIGN);
	}
}