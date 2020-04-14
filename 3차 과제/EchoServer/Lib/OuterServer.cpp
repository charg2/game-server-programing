
#include "OuterServer.h"
#include "../../Common/SocketAddress.h"
#include "../../Common/exception.h"

#include "KeyManager.h"

OuterServer::OuterServer() : listen_socket{ INVALID_SOCKET }, generated_session_id{ }, version { c2::constant::version }
, last_error{}, custom_error_code{ c2::enumeration::ERROR_NONE }, threads{}, is_running{}, srw_lock{}

{}

OuterServer::~OuterServer()
{}

bool OuterServer::initialize()
{
	printf("OuterServer::initialize()...");
	if (false == on_initialize_before_init())
	{
		c2::util::crash_assert();
	}

	InitializeSRWLock(&this->srw_lock);
	generated_session_id = 0;

	init_network();
	
	printf("ok\n");

	return on_initialize_after_init();
}

void OuterServer::finalize()
{
	on_finalize_before_fin();
	
	closesocket(this->listen_socket);
	
	WSACleanup();

	// thread 정리.
	for ( std::thread* thread : threads)
	{
		thread->join();
	}

	on_finalize_after_fin();
}

void OuterServer::disconnect_session(Session* session)
{
	if (0 != session->refer_count)
	{
		return;
	}

/////////////////////////////////////////////////////////////////////////////
	AcquireSRWLockExclusive(&srw_lock);									   //
																		   //
	session_map.erase(session->unique_session_id);						   //
	sessions.erase(std::remove(sessions.begin(), sessions.end(), session));//
																		   //
	ReleaseSRWLockExclusive(&srw_lock);									   //
/////////////////////////////////////////////////////////////////////////////

	CancelIoEx((HANDLE)session->socket, NULL);

	on_disconnect(session);

	closesocket(session->socket);

	printf("OuterServer::disconnect  session_id : %d \n ", session->unique_session_id);

	delete session;
}


void OuterServer::start()
{
	printf("OuterServer::start()...");

	KeyManager key_manager{};

	start_thread();

	//for (std::thread* thread : threads)
	//{
		/*
		if (true == thread->joinable())
		{
			thread->join();
		}
		else*/
		//{
		//	Sleep(100);

		//	printf("t\n");

		//}
	//}
	printf("ok\n");

	for (;;)
	{
		{
			Sleep(50);

			if (key_manager.key_down(VirtualKey::A))
			{
				int bsize;
				int rn = sizeof(bsize);
				getsockopt(listen_socket, SOL_SOCKET, SO_RCVBUF, (char*)&bsize, (socklen_t*)&rn);
				printf("recv buf size : %d\n", bsize);
			}
			else if (key_manager.key_down(VirtualKey::B))
			{
				printf("bye");
			}
		}
	}

}

bool OuterServer::on_initialize_after_init()
{
	return true;
}

bool OuterServer::on_initialize_before_init()
{
	return true;
}

void OuterServer::on_connect(Session* session)
{
}



void OuterServer::on_disconnect(Session* session)
{


}

bool OuterServer::on_accept(Session* session)
{
	return true;
}


void OuterServer::on_recv()
{
}

void OuterServer::on_finalize_after_fin()
{
}

void OuterServer::on_finalize_before_fin()
{
}


Session* OuterServer::create_session()
{
	return nullptr;
}


const std::string_view& OuterServer::get_version() const
{
	return this->version;
}


c2::enumeration::ErrorCode OuterServer::get_error_code()
{
	return this->custom_error_code;
}


void OuterServer::set_error_code(c2::enumeration::ErrorCode err_code)
{
	this->custom_error_code = err_code;
}

void OuterServer::on_update()
{
}



size_t OuterServer::get_ccu()
{
	return sessions.size();
}

std::vector<Session*>& OuterServer::get_sessions()
{
	return sessions;
}

void OuterServer::start_thread()
{
	using namespace c2::enumeration;

	//this->io_thread		= new std::thread{ OuterServer::io_thread_function, this };
	//this->accept_thread = new std::thread{ OuterServer::accept_thread_function, this };
	
	for (int n = 0; n < TT_MAX; ++n)
	{
		threads[n] = new std::thread{ run, this, static_cast<c2::enumeration::ThreadType>(n) };
	}
}

bool OuterServer::init_network()
{
	WSAData wsa;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		printf("OuterServer::init() WSAStartup() failure \n");
		return false;
	}

	listen_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == this->listen_socket)
	{
		printf("OuterServer::init()::socket() create listen socket failure \n");
		return false;
	}

	// linger
	linger linger_opt{ 1, 0 };
	if (SOCKET_ERROR == setsockopt(listen_socket, SOL_SOCKET, SO_LINGER, (char*)&linger_opt, sizeof(LINGER)))
	{
		return false;
	}

	if (true == c2::global::enable_nagle_opt) //네이글 알고리즘 on/off
	{
		int option = TRUE;
		setsockopt(this->listen_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));
	}

	/////////////////// for test
	int bsize = 0;
	int rn = 4;
	if (SOCKET_ERROR == getsockopt(listen_socket, SOL_SOCKET, SO_RCVBUF, (char*)&bsize, (socklen_t*)&rn))
		return false;
	printf("Send buf size : %d\n", bsize);

	bsize = 0;
	setsockopt(listen_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&bsize, (socklen_t)rn);
	getsockopt(listen_socket, SOL_SOCKET, SO_RCVBUF, (char*)&bsize, (socklen_t*)&rn);
	printf("Send buf size : %d\n", bsize);

	/////////////////// for test

	DWORD bytesReturned{ 0 };
	tcp_keepalive option{ true, 10000, 1000 };
	WSAIoctl(listen_socket, SIO_KEEPALIVE_VALS, &option, sizeof(tcp_keepalive), 0, 0, &bytesReturned, NULL, NULL);

	SocketAddress sock_addr{ (uint32_t)INADDR_ANY , c2::constant::server_port };
	if ( SOCKET_ERROR == ::bind(this->listen_socket, sock_addr.get_as_sockaddr(), sock_addr.size()) )
	{
		return false;
	}

	if (SOCKET_ERROR == listen(listen_socket, SOMAXCONN))
	{
		return false;
	}

	return true;
}



void OuterServer::io_thread_function()
{
	// for newbie...
	std::vector<Session*> local_temp_vector;

	for (;;)
	{

//CRITICAL_SECTION///////////////CRITICAL_SECTION//////////
		AcquireSRWLockExclusive(&srw_lock);				 //
														 //
		while (false == new_sessions.empty())			 //
		{												 //
			Session* session = this->new_sessions.back();//
			this->new_sessions.pop();					 //
			local_temp_vector.push_back(session);		 //
		}												 //
														 //
		ReleaseSRWLockExclusive(&srw_lock);				 //
///////////////CRITICAL_SECTION////////////////////CRITICAL


		//	post_recv
		while (false == local_temp_vector.empty())
		{
			Session* session = local_temp_vector.back();

			session->increase_reference();

			session->post_recv();

			local_temp_vector.pop_back();

			sessions.push_back(session);

			if (false == session_map.insert(std::unordered_map<size_t, Session*>::value_type(session->unique_session_id, session)).second)
			{
				printf("duplicated session key  : %d", session->unique_session_id);
				this->custom_error_code = c2::enumeration::ErrorCode::ERROR_DUPLICATED_SESSION_KEY;
				c2::util::crash_assert();
			}
		}

		// send
		size_t session_cnt = sessions.size();
		for (size_t n{}; n < session_cnt; ++n)
		{
			// 전송중이 아님. overlapped 처리중...
			if (false == sessions[n]->send_flag )
			{
				sessions[n]->increase_reference();

				sessions[n]->post_send(); // 처리중..ㄷ
			}
		}

		// IO처리 다하고.
		// http://egloos.zum.com/himskim/v/1053865
		SleepEx(30, TRUE); // 실행.

		on_update();
	}
}

void OuterServer::accept_thread_function()
{
	for (;;)
	{
		SOCKADDR sock_addr{};
		int sock_addr_len{ sizeof(sockaddr) };

		for (;;)
		{
			Session* session = this->create_session();

			session->socket = accept(listen_socket, &session->sock_addr, &sock_addr_len);
			if (INVALID_SOCKET == session->socket)
			{
				last_error = GetLastError();
				delete session;
				printf("accept error : %d \n", last_error);
			}
			if (false == on_accept(session))
			{
				//error loging
				delete session;
				continue;
			}

			session->unique_session_id = generated_session_id;

			generated_session_id += 1;

			AcquireSRWLockExclusive(&srw_lock);

			new_sessions.push(session);

			ReleaseSRWLockExclusive(&srw_lock);
			
			printf("accept_thread_function()");
		}

	}
}

void OuterServer::run(OuterServer* server, c2::enumeration::ThreadType thread_type)
{
	using namespace c2::enumeration;

	switch (thread_type)
	{
	case ThreadType::TT_IO_THREAD:
		server->io_thread_function();
		return;

	case ThreadType::TT_ACCEPT_THREAD:
		server->accept_thread_function();
		return;

	default:
		c2::util::crash_assert();
		break;
	}
}


