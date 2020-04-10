#pragma once

#include <string_view>

namespace c2::global
{
	constexpr bool enable_nagle_opt	{};
}

namespace c2::constant
{
	constexpr std::string_view version		{ "overlapped IO server" };
	constexpr std::string_view server_ip	{ "127.0.0.1" };
	constexpr uint16_t server_port			{ 41322 };
	constexpr size_t   session_buffer_size	{ 65536 };
	
	constexpr int8_t  player_spwan_point_x	{ 3 };
	constexpr int8_t  player_spwan_point_y	{ 3 };
}

namespace c2::enumeration
{
	enum ErrorCode
	{
		ERROR_NONE,

		// SOCK LIB
		ERROR_CLIENT_SOCKET_CONNECTION_FAILURE = 1000,
		ERROR_SOCKET_IS_CONNECTING,
		ERROR_SOCKET_CREATION_FAILURE,

		// TO APPLY SOCKET OPT
		ERROR_FAILURE_TO_APPLY_NODELAY_OPT = 2000,
		ERROR_FAILURE_TO_APPLY_LINGER_OPT,
		ERROR_FAILURE_TO_APPLY_NONE_BLOCK_OPT,
		ERROR_FAILURE_TO_APPLY_KEEPALIVE_OPT,

		// ENGINE
		ERROR_WINSOCK_LIB_INIT_FAILURE = 3000,
		ERROR_DUPLICATED_SESSION_KEY,

		// 
		ERROR_NONE_WOULD_BLOCK
	};

	enum DisconnectReason
	{
		DR_NONE,
		DR_CONNECTION_IS_DELAYED,
		DR_RECV_BUFFER_FULL,
		DR_SEND_BUFFER_FULL,
		DR_FROM_SERVER,
		DR_FROM_CLIENT,
		DR_REQEUSET_FROM_CLIENT,
	};

	enum SessionState
	{
		SS_NONE,
		SS_CONNECTING,
		SS_CONNECTED,
		SS_DISCONNECTED,
		//SS_RecvBufferFull,
		//SS_SendBufferFull,
	};

	enum ThreadType
	{
		TT_IO_THREAD,
		TT_ACCEPT_THREAD,
		TT_MAX,
	};
}

