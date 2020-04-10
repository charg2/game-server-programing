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

}

namespace c2::enumeration
{
	enum ErrorCode
	{
		Error_None,

		// Sock Lib
		Error_Client_Socket_Connection_Failure = 1000,
		Error_Socket_Is_Connecting,
		Error_Socket_Creation_Failure,

		// To Apply Socket Opt
		Error_Failure_To_Apply_NoDelay_Opt = 2000,
		Error_Failure_To_Apply_Linger_Opt,
		Error_Failure_To_Apply_None_Block_Opt,
		Error_Failure_To_Apply_KeepAlive_Opt,

		// Engine
		Error_winsock_lib_init_failure = 3000,
		Error_Duplicated_Session_Key,

		Error_None_WOULD_BLOCK
	};

	enum DisconnectReason
	{
		DR_None,
		DR_Connection_Is_Delayed,
		DR_FromServer,
		DR_FromClient,
	};

	enum SessionState
	{
		SS_None,
		SS_Connecting,
		SS_Connected,
		SS_Disconnected,
		//SS_RecvBufferFull,
		//SS_SendBufferFull,
	};

	enum ThreadType
	{
		TT_IO_Thread,
		TT_Accept_Thread,
		TT_Max,
	};
}

