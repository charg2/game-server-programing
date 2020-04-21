#include "../../Common/Packet.hpp"
#include "../../Common/SocketAddress.h"
#include "JsonParser.h"
#include "IOContext.h"
#include "Session.h"
#include "OuterServer.h"

LPFN_ACCEPTEX		OuterServer::accept_ex				{nullptr};
LPFN_DISCONNECTEX	OuterServer::disconnect_ex			{nullptr};
LPFN_CONNECTEX		OuterServer::connect_ex				{nullptr};
thread_local size_t OuterServer::local_storage_accessor {};

OuterServer::OuterServer() : listen_sock{ INVALID_SOCKET }, completion_port{ INVALID_HANDLE_VALUE }, accepter{ INVALID_HANDLE_VALUE }
, io_handler{ nullptr }, port{ 0 }, sessions{ }
, custom_server_error{ c2::enumeration::ER_NONE }, custom_kernel_error{ c2::enumeration::ER_NONE } // error'
, concurrent_connected_user{ 0 }, concurrent_thread_count { 0 }
, version{} 
{}

OuterServer::~OuterServer()
{}

bool OuterServer::init_network()
{
	WSAData wsa;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		printf("OuterServer::init() WSAStartup() failure \n");
		//console_log();
		this->custom_kernel_error = c2::enumeration::ER_COMPLETION_PORT_INITIATION_FAILURE;

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

	if (true == c2::global::enable_nagle_opt) //네이글 알고리즘 on/off
	{
		int option = TRUE;
		setsockopt(this->listen_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));
	}


	DWORD bytesReturned{ 0 };
	tcp_keepalive option{ true, 10000, 1000 };
	WSAIoctl(listen_sock, SIO_KEEPALIVE_VALS, &option, sizeof(tcp_keepalive), 0, 0, &bytesReturned, NULL, NULL);

	SocketAddress sock_addr{ this->ip, this->port };
	if (SOCKET_ERROR == ::bind(this->listen_sock, sock_addr.get_as_sockaddr(), sock_addr.size()))
	{
		return false;
	}

	return true;
}

bool OuterServer::init_sessions()
{
	this->completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE,);

	Session* created_sessions = create_sessions();

	for (int n = 0; n < 5000; ++n)
	{
		this->sessions[n] = &created_sessions[n];
		this->sessions[n]->session_id = n;
		this->sessions[n]->server = this;
	}

	return true;
}

bool OuterServer::init_threads()
{
	total_recv_bytes = new uint64_t[concurrent_thread_count] {};
	total_recv_count = new uint64_t[concurrent_thread_count] {};
	total_sent_bytes = new uint64_t[concurrent_thread_count] {};
	total_sent_count = new uint64_t[concurrent_thread_count] {};

	uint64_t n = 0;
	for (; n < this->concurrent_thread_count; ++n )
	{
		void* params = new void* [3]{ (void*)this, (void*)c2::enumeration::ThreadType::TT_IO, (void*)n};

		accepter = (HANDLE)_beginthreadex(NULL, NULL, OuterServer::start_thread, params, NULL, NULL);
	}



	// 접속을 나중에 받기 위해 ㅇㅇ
	void* params = new void* [3]{ (void*)this, (void*)c2::enumeration::ThreadType::TT_ACCEPTER, (void*)n };
	accepter = (HANDLE)_beginthreadex(NULL, NULL, OuterServer::start_thread, params ,NULL, NULL );

	return true;
}

bool OuterServer::init_system()
{
	if (NULL == CreateIoCompletionPort(this->completion_port, INVALID_HANDLE_VALUE, NULL, this->concurrent_thread_count))
	{
		this->custom_kernel_error = c2::enumeration::ER_COMPLETION_PORT_INITIATION_FAILURE;
		return false;
	}

	// Associate the listening socket with the completion port
	CreateIoCompletionPort((HANDLE)this->listen_sock, this->completion_port, (u_long)0, 0);


	DWORD bytes = 0;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	if (SOCKET_ERROR == WSAIoctl(listen_sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&OuterServer::accept_ex, sizeof(OuterServer::accept_ex),
		&bytes, NULL, NULL))
	{
		this->custom_kernel_error = c2::enumeration::ER_ACCEPTEX_LAODING_FAILURE;
		return false;
	}

	bytes = 0;
	GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
	if (SOCKET_ERROR == WSAIoctl(listen_sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidDisconnectEx, sizeof(GuidDisconnectEx),
		&OuterServer::disconnect_ex, sizeof(OuterServer::disconnect_ex),
		&bytes, NULL, NULL))
	{
		this->custom_kernel_error = c2::enumeration::ER_ACCEPTEX_LAODING_FAILURE;
		return false;
	}


	bytes = 0;
	GUID GuidConnectEx = WSAID_CONNECTEX;
	if (SOCKET_ERROR == WSAIoctl(listen_sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidConnectEx, sizeof(GuidConnectEx),
		&OuterServer::connect_ex, sizeof(OuterServer::connect_ex),
		&bytes, NULL, NULL))
	{
		this->custom_kernel_error = c2::enumeration::ER_CONNECTEX_LAODING_FAILURE;
		return false;
	}

	return true;
}


void OuterServer::accepter_procedure(uint64_t idx)
{	 
	SOCKET		local_listen_sock = this->listen_sock;
	uint64_t	accepted_counter  = 0;
	if (SOCKET_ERROR == listen(local_listen_sock, SOMAXCONN_HINT(this->max_listening_count)))
	{
		this->custom_kernel_error = c2::enumeration::ER_ACCEPTEX_LAODING_FAILURE//;
		return;
	}

	// concurrent_stack 을통해서 꺼냄.

	for ( ; accepted_counter < this->capacity; )
	{
		// 세션을 얻고...

		Sleep(100);
	}
}


void OuterServer::io_service_procedure(uint64_t id)
{
	OuterServer::local_storage_accessor = id;
	HANDLE	local_completion_port	{ this->completion_port };
	int64_t	thread_id				{ GetCurrentThreadId() };


	for (;;)
	{
		DWORD			transfered_bytes{};
		LPOVERLAPPED	overlapped_ptr{};
		ULONG_PTR		completion_key{};
		
		bool ret = GetQueuedCompletionStatus(local_completion_port, &transfered_bytes, &completion_key, &overlapped_ptr, INFINITE);
		if (overlapped_ptr == nullptr && completion_key == 0 && transfered_bytes == 0)
		{
			 // 종료 조건.
			break;
		}

		Session*   session	= acquire_session_ownership(completion_key); 
		
		// acquire session
		if ((size_t)overlapped_ptr == c2::constant::SEND_SIGN)
		{
			session->post_send();
			continue;
		}

		IoContext* context_ptr	{ reinterpret_cast<IoContext*>(overlapped_ptr) };

		switch (context_ptr->io_type)
		{
		case IO_RECV:
			session->recv_completion(transfered_bytes);
			break;
		case IO_SEND:
			session->send_completion(transfered_bytes);
			break;
		case IO_ACCEPT:
			session->accept_completion();
			break;
		case IO_DISCONNECT:
			session->disconnect_completion();
			break;
		default:
			c2::util::crash_assert();
			break;
		}

		// release session
		release_session_ownership(completion_key);
	}
}




uint32_t WINAPI OuterServer::start_thread(LPVOID param)
//void OuterServer::start_thread(OuterServer* server, c2::enumeration::ThreadType thread_type, void* param)
{			
	using namespace c2::enumeration;

	ThreadInfo* info = reinterpret_cast<ThreadInfo*>(param);

	switch (info->thread_tye)
	{
		case ThreadType::TT_IO:
			info->server->io_service_procedure(info->index);
			break;
		
		case ThreadType::TT_ACCEPTER:
			info->server->accepter_procedure(info->index);
			break;

		case ThreadType::TT_CUSTOM:
			info->server->custom_precedure(info->index);
			break;

		default:
			c2::util::crash_assert();
			break;
	}

	delete info;
}


void OuterServer::load_config_using_json(const wchar_t* file_name)
{
	c2::util::JsonParser json_file;
	
	do
	{
		if (false == json_file.load_json(file_name))
			break;
			
		if (false == json_file.get_raw_wstring(L"server_version", this->version, count_of(version)))
			break;

		if (false == json_file.get_boolean(L"enable_nagle_opt", this->nagle_opt))
			break;

		if (false == json_file.get_boolean(L"enabled_keep_alive_opt", this->keep_alive_opt))
			break;

		if (false == json_file.get_uint64(L"concurrent_thread_count", this->concurrent_thread_count))
			break;

		if (false == json_file.get_uint16(L"server_port", this->port))
			break;

		if (false == json_file.get_raw_wstring(L"server_ip", this->ip, count_of(ip) ))
			break;

		if (false == json_file.get_uint16(L"capacity", this->capacity))
			break;

		if (false == json_file.get_uint16(L"max_listening_count", this->max_listening_count))
			break;

		return;

	} while (false);


	c2::util::crash_assert();
}

bool OuterServer::initialize()
{
	do
	{
		if (false == init_sessions())
			break;

		if (false == init_network())
			break;

		if (false == init_threads())
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
	// session에서 하고 
	if (FALSE == OuterServer::disconnect_ex(session->sock, &session->discon_context.overalpped, TF_REUSE_SOCKET, ))
	{

	}

	// 서버 처리.
	on_disconnect(session);
}


void OuterServer::start()
{
}

void OuterServer::stop()
{
}

void OuterServer::on_connect(Session* session)
{
}

void OuterServer::on_disconnect(Session* session)
{
}

Session* OuterServer::create_sessions(size_t n)
{
	return reinterpret_cast<Session*>(new Session[n]);
}

void OuterServer::disconnect(uint64_t session_id)
{
	Session* session = acquire_session_ownership(session_id);

	if (session->io_refer_count != 1)
	{
		debug_console("OuterServer::disconnect() : session_id : %d, ref_count : %d", session->session_id, session->io_refer_count);
		c2::util::crash_assert();
	}


	on_disconnect(session);

	release_session_ownership(session_id);
	// 스택에 넣어주고.
	// push.(session_id);
}

void OuterServer::on_wake_io_thread()
{
}

void OuterServer::on_sleep_io_thread()
{
}

void OuterServer::disconnect_after_sending(uint64_t session_id)
{
}


const wchar_t* OuterServer::get_version() const
{
	return version;
}

const c2::enumeration::ErrorCode OuterServer::get_custom_last_error() const
{
	return c2::enumeration::ErrorCode();
}


void OuterServer::set_custom_last_error(c2::enumeration::ErrorCode err_code)
{
	this->custom_last_error = err_code;
}

size_t OuterServer::get_toatl_recv_bytes()
{
	size_t total = 0;

	for (size_t n = 0; n < this->concurrent_thread_count; ++n)
	{
		total += total_recv_bytes[n];
	}

	return total;
}

size_t OuterServer::get_toatl_sent_bytes()
{
	size_t total = 0;

	for (size_t n = 0; n < this->concurrent_thread_count; ++n)
	{
		total += total_sent_bytes[n];
	}

	return total;
}


Session* OuterServer::acquire_session_ownership(int64_t index)
{
	Session* session = sessions[(uint16_t)index];
	
	session->increase_refer();
	
	if (index != session->session_id) // 동일한 녀석인지 확인.
	{
		// 문제가 발생한 상황.
		c2::util::crash_assert();
	}

	// disconnectEx를 걸어야 하나;
	
	
}

void OuterServer::release_session_ownership(int64_t index)
{

}



constexpr size_t OuterServer::get_ccu() const
{
	return concurrent_connected_user;
}


