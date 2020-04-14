#include "OuterServer.h"

LPFN_ACCEPTEX		OuterServer::acceptex		{nullptr};
LPFN_DISCONNECTEX	OuterServer::disconnectex	{nullptr};
LPFN_CONNECTEX		OuterServer::connectex		{nullptr};


bool OuterServer::init_network()
{
	WSAData wsa;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		//printf("OuterServer::init() WSAStartup() failure \n");
		return false;
	}

	listen_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == this->listen_sock)
	{
		//printf("OuterServer::init()::socket() create listen socket failure \n");
		return false;
	}

	// linger
	linger linger_opt{ 1, 0 };
	if (SOCKET_ERROR == setsockopt(listen_sock, SOL_SOCKET, SO_LINGER, (char*)&linger_opt, sizeof(LINGER)))
	{
		return false;
	}

	//if (true == c2::global::enable_nagle_opt) //네이글 알고리즘 on/off
	//{
	//	int option = TRUE;
	//	setsockopt(this->listen_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));
	//}


	DWORD bytesReturned{ 0 };
	tcp_keepalive option{ true, 10000, 1000 };
	WSAIoctl(listen_sock, SIO_KEEPALIVE_VALS, &option, sizeof(tcp_keepalive), 0, 0, &bytesReturned, NULL, NULL);

	//SocketAddress sock_addr{ (uint32_t)INADDR_ANY , c2::constant::server_port };
	//if (SOCKET_ERROR == ::bind(this->listen_sock, sock_addr.get_as_sockaddr(), sock_addr.size()))
	//{
	//	return false;
	//}

	DWORD bytes = 0;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	if ( SOCKET_ERROR == WSAIoctl(listen_sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&OuterServer::acceptex, sizeof(OuterServer::acceptex),
		&bytes, NULL, NULL) )
	{
		
		this->custom_last_error = c2::enumeration::ER_ACCEPTEX_LAODING_FAILURE;
		return false;
	}

	bytes = 0;
	GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
	if (SOCKET_ERROR == WSAIoctl(listen_sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidDisconnectEx, sizeof(GuidDisconnectEx),
		&OuterServer::disconnectex, sizeof(OuterServer::disconnectex),
		&bytes, NULL, NULL))
	{
		this->custom_last_error = c2::enumeration::ER_ACCEPTEX_LAODING_FAILURE;
		return false;
	}


	bytes = 0;
	GUID GuidConnectEx = WSAID_CONNECTEX;
	if (SOCKET_ERROR == WSAIoctl(listen_sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidConnectEx, sizeof(GuidConnectEx),
		&OuterServer::connectex, sizeof(OuterServer::connectex),
		&bytes, NULL, NULL))
	{

		return false;
	}

	if (SOCKET_ERROR == listen(listen_sock, SOMAXCONN))
	{
		return false;
	}




	return true;

}

bool OuterServer::init_sessions()
{

	this->completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE,);

	return true;
}



void OuterServer::accepter_procedure()
{	 
	SOCKET local_listen_sock = this->listen_sock;
	
	if (SOCKET_ERROR == listen(local_listen_sock, SOMAXCONN))
	{
		return;
	}

	for (;;)
	{
		// 세션을 얻고...

		// overlapped 구조체
		WSAOVERLAPPED overlapped;
		
		overlapped.Internal = 0;
		overlapped.InternalHigh = 0;

		if (FALSE == OuterServer::acceptex(local_listen_sock, session->sock, , , , , , &overlapped) )
		{
			DWORD local_last_error = GetLastError();
			if (WSA_IO_PENDING != local_last_error)
			{
				// 종료 처리.
				wprintf(L"AcceptEx failed with error: %u\n", WSAGetLastError());

			}

			// 그냥ㄹ 리턴
		}

	}
}

void OuterServer::io_service_procedure()
{
	HANDLE	local_completion_port	{ this->completion_port };
	int64_t	thread_id				{ GetCurrentThreadId() };

	for (;;)
	{
		DWORD			transfered_bytes;
		LPOVERLAPPED	overlapped;
		ULONG_PTR		completion_key;
		
		bool ret = GetQueuedCompletionStatus(local_completion_port, &transfered_bytes, &completion_key, &overlapped, INFINITE);
	}

}

void OuterServer::start_thread(OuterServer* server, c2::enumeration::ThreadType thread_type)
{
	using namespace c2::enumeration;

	switch (thread_type)
	{
	case ThreadType::TT_IO:
		server->io_service_procedure();
		return;

	case ThreadType::TT_ACCEPTER:
		server->accepter_procedure();
		return;

	case ThreadType::TT_CUSTOM:
		server->custom_precedure();
		return;

	default:
		c2::util::crash_assert();
		break;
	}


}


OuterServer::OuterServer()
{

}

bool OuterServer::initialize()
{
	do
	{
		if (false == init_sessions())
			break;

		if (false == init_network())
			break;

		return true;

	} while (false);

	return false;
}

void OuterServer::finalize()
{

}

void OuterServer::disconnect_session(Session* session)
{
	
	if (FALSE == OuterServer::disconnectex(session->sock, &session->discon_context.overlapped, TF_REUSE_SOCKET, ))
	{

	}

	on_disconnect(session);
}


void OuterServer::start()
{

}

void OuterServer::on_connect(Session* session)
{
}

void OuterServer::on_disconnect(Session* session)
{
}


const wchar_t* OuterServer::get_version() const
{
	return version;
}

c2::enumeration::ErrorCode OuterServer::get_error_code() const
{
	return c2::enumeration::ErrorCode();
}


void OuterServer::set_error_code(c2::enumeration::ErrorCode err_code)
{
	this->custom_last_error = err_code;
}

constexpr size_t OuterServer::get_ccu() const
{
	return concurrent_connected_user;
}


