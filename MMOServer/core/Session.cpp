#include "../pre_compile.h"
#include "OuterServer.h"
#include "Session.h"
#include "DBTask.h"

using namespace c2::enumeration;

Session::Session() :
refer_count{ 0 }, send_flag{ 0 }, packet_sent_count{ 0 },
recv_packet{ }, 
accept_context{ {}, IO_ACCEPT, this}, send_context{ {},  IO_SEND, this },
recv_context{ {},  IO_RECV, this }, discon_context{ {},IO_DISCONNECT, this },
sock_addr{}, total_recv_bytes{}, total_sent_bytes{},
release_flag{}, io_cancled{}, sock{ INVALID_SOCKET }, server{ nullptr }
{
	this->sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

Session::~Session(){}

static char global_buffer[sizeof sockaddr_in * 2 + 32];

void Session::post_accept()
{
	this->increase_refer();

	DWORD bytes = 0;
	DWORD flags = 0;

	accept_context.overalpped.Internal = 0;
	accept_context.overalpped.InternalHigh = 0;

	if (FALSE == OuterServer::accept_ex(server->listen_sock, sock, global_buffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, &accept_context.overalpped))
	{
		DWORD local_last_error = GetLastError();
		if (WSA_IO_PENDING != local_last_error)
		{
			debug_code( wprintf(L"AcceptEx failed with error: %u\n", WSAGetLastError()) );
		}
		// 그냥ㄹ 리턴

		// 실패했다면 
		// 성능을 위해선 reset하고 다시 사용. 

		// 어차피 스택에 들가면 다시 사용됨.
	}
}

void Session::post_recv()
{
	WSABUF		wsa_bufs[2];
	uint32_t	buffer_count;
	size_t		first_space_size = recv_buffer.direct_enqueue_size();
	uint32_t	flag{};

	if (first_space_size < recv_buffer.get_free_size())
	{
		wsa_bufs[0].buf = recv_buffer.get_front_buffer();
		wsa_bufs[0].len = first_space_size;

		wsa_bufs[1].buf = recv_buffer.get_buffer();
		wsa_bufs[1].len = recv_buffer.get_free_size() - first_space_size;
		
		buffer_count = 2;
	}
	else
	{
		wsa_bufs[0].buf = recv_buffer.get_front_buffer();
		wsa_bufs[0].len = first_space_size;

		buffer_count = 1;
	}

	// 여기부 생길 오류는 nullptr reference or buffer문제...
	this->recv_context.overalpped.Internal = 0;
	this->recv_context.overalpped.InternalHigh = 0;


	int64_t ret_val = WSARecv(sock, wsa_bufs, buffer_count, NULL, (LPDWORD)&flag, &this->recv_context.overalpped, NULL);
	if (SOCKET_ERROR == ret_val)	
	{
		uint32_t local_last_error = GetLastError();
		if (WSA_IO_PENDING != local_last_error)
		{
			//debug_code( printf("[ERROR] %s, %s, %s : %d \n", __FILE__, __LINE__ , __FUNCSIG__, local_last_error) );
			
			if (1 > InterlockedDecrement64(&this->refer_count))
			{

				if (1 == InterlockedExchange(&this->io_cancled, 1))
				{
					request_disconnection();
				}
				else
				{
					CancelIoEx(reinterpret_cast<HANDLE>(sock), NULL);
				}
			}
			return;
		}
	}

	return;
}

void Session::request_disconnection()
{
	// session에서 하고 
	if (FALSE == OuterServer::disconnect_ex(sock, &discon_context.overalpped, TF_REUSE_SOCKET, 0))
	{
		DWORD local_last_error = GetLastError();
		if (WSA_IO_PENDING == local_last_error) // 봐준다..
		{

		}
	}
}

void Session::post_send()
{
	for (;;)
	{
		if (1 == InterlockedExchange64(&send_flag, 1))
		{
			return;
		}

		// 패킷 감지를 못함.
		if ( send_buffer.unsafe_size() == 0 )
		{
			send_flag = 0;

			if (send_buffer.unsafe_size() > 0)
				continue;

			return;
		}

		size_t	send_packet_count = 0;
		WSABUF	wsa_bufs[c2::constant::MAX_CONCURRENT_SEND_COUNT];
		DWORD	flag{};

		for (; (send_packet_count < c2::constant::MAX_CONCURRENT_SEND_COUNT) && this->send_buffer.try_pop(this->sent_packets[send_packet_count]) ; ++send_packet_count)
		{
			wsa_bufs[send_packet_count].buf = sent_packets[send_packet_count]->get_buffer();
			wsa_bufs[send_packet_count].len = sent_packets[send_packet_count]->size();
		}
		

		// 여기 이후에 send가 보내진 애들은 어캄?
		send_context.overalpped.Internal = 0;
		send_context.overalpped.InternalHigh = 0;

		this->packet_sent_count = send_packet_count;

		InterlockedIncrement64(&this->refer_count); 
		
		int64_t ret_val = WSASend(this->sock, wsa_bufs, send_packet_count, NULL, flag, &this->send_context.overalpped, NULL);
		if (SOCKET_ERROR == ret_val)
		{
			uint32_t local_last_error = GetLastError();
			if (WSA_IO_PENDING != local_last_error)
			{
				//debug_console(printf("ret_val of WSASend() is not IO_PENDING  : %d \n", local_last_error));
				printf("WSASend() failure : %llu,  error_code : %llu \n", session_id, local_last_error);
				if ( 1 > InterlockedDecrement64(&this->refer_count)) // 
				{
					//this->decrease_refer();
					if (1 == InterlockedExchange(&this->io_cancled, 1))
					{
						request_disconnection();
					}
					else
					{
						CancelIoEx(reinterpret_cast<HANDLE>(sock), NULL);
					}
				}
				return;
			}
		}
	}
}


void Session::recv_completion(size_t transfered_bytes)
{
	this->recv_buffer.move_front(transfered_bytes);
	if (0 == transfered_bytes)
	{
		if (0 == InterlockedDecrement64(&this->refer_count))
		{
			server->request_disconnection(this->session_id, DR_RECV_BUFFER_FULL);
		}

		return;
	}

	if (0 == this->recv_buffer.get_free_size() )
	{
		if (0 == InterlockedDecrement64(&this->refer_count))
		{
			server->request_disconnection(this->session_id, DR_RECV_BUFFER_FULL);
		}

		return;
	}


	this->parse_packet();
	//this->parse_packet_echo();

	InterlockedAdd64(&server->total_recv_bytes, transfered_bytes);

	InterlockedIncrement64(&server->total_recv_count);

	this->post_recv();
}

void Session::send_completion(size_t transfered_bytes)
{
	//server->total_sent_bytes[server->local_storage_accessor] += transfered_bytes;
	uint64_t temp_packet_count = this->packet_sent_count;
	for (size_t n = 0; n < temp_packet_count; ++n)
		sent_packets[n]->decrease_ref_count();

	this->packet_sent_count = 0;

	this->send_flag = 0;

	this->post_send();

	InterlockedAdd64(&server->total_sent_bytes, transfered_bytes);

	InterlockedIncrement64(&server->total_sent_count);

	server->release_session_ownership( this->session_id);
}

void Session::accept_completion()
{
	HANDLE returned_hanlde = CreateIoCompletionPort((HANDLE)this->sock, server->completion_port, session_id, 0);
	if(returned_hanlde == NULL ||  returned_hanlde != server->completion_port)
	{
		debug_code(printf("[ERROR] OuterServer::CompletionPort Bind() failed"););

		this->last_error = ER_ASSOCIATIVE_COMPLETION_PORT_FALIURE;
	}


	// recv 용 ref count 
	//InterlockedIncrement64(&this->refer_count);
	//if( 0 == InterlockedDecrement64(&this->refer_count)) // accpet count 
	//{
	//}

	InterlockedIncrement(&server->current_accepted_count); 
	
	// 인터락 증감을 하지 않고 대로 사용함.
	

	this->post_recv();
}

void Session::disconnect_completion()
{
	// 컨텐츠 통지
	server->on_disconnect(this->session_id); // 

	// session에서 하고 
	decrease_refer();

	// io 정리 완료된 상태.
	//this->session_id = increase_session_stamp(session_id);

	//server->id_pool.push(this->session_id);
}

void Session::parse_packet()
{
	using namespace c2::enumeration;

	c2::Packet* local_packet = &recv_packet;
	local_packet->clear();

	PacketHeader header{};

	for (;;)
	{
		size_t payload_length = recv_buffer.get_use_size();
		if (sizeof(PacketHeader) > payload_length)
		{
			return;
		}

		recv_buffer.peek((char*)&header, sizeof(PacketHeader));
		if (header.length > payload_length)
		{
			return;
		}

		if (header.type < PT_NONE && PT_MAX <= header.type)
		{
			c2::util::crash_assert();
		}

		local_packet->write(recv_buffer.get_rear_buffer(), header.length);

		handler_table[header.type](this, header, *local_packet);

		local_packet->rewind();

		recv_buffer.move_rear(header.length);
	}
}

//void Session::parse_packet_echo()
//{
//	using namespace c2::enumeration;
//
//	c2::Packet* local_packet = &recv_packet;
//	local_packet->clear();
//	PacketHeader temp{};
//	uint16_t header{};
//
//	for (;;)
//	{
//		size_t payload_length = recv_buffer.get_use_size();
//		if (sizeof(uint16_t) > payload_length)
//		{
//			return;
//		}
//
//		recv_buffer.peek( (char*)&header, sizeof(uint16_t) );
//		if ( ( header + sizeof(header)) > payload_length)
//		{
//			return;
//		}
//
//		header += sizeof(header);
//
//		size_t direct_deque_size = recv_buffer.direct_dequeue_size();
//		if (direct_deque_size >= header)
//		{
//			local_packet->write(recv_buffer.get_rear_buffer(), header);
//		}
//		else
//		{
//			local_packet->write(recv_buffer.get_rear_buffer(), direct_deque_size);
//			local_packet->write(recv_buffer.get_buffer(),  header - direct_deque_size);
//		}
//
//		handler_table[c2::enumeration::PacketType::PT_CS_ECHO](this, temp, *local_packet);
//
//		local_packet->clear();
//
//		recv_buffer.move_rear(header);
//	}
//}


void Session::reset()
{
	this->sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	refer_count = 0;
	recv_packet.clear();
	total_recv_bytes = 0;
	total_sent_bytes = 0;
	send_flag = 0;
	packet_sent_count = 0;
	release_flag = 0;  
	io_cancled = 0;
}

void Session::increase_refer()
{
	if ( 0 >= InterlockedIncrement64(&refer_count))
	{
		this->last_error = 0U;
		c2::util::crash_assert();
	}
}

void Session::decrease_refer()
{
	uint64_t ret_val = InterlockedIncrement64(&refer_count);
	if (1 > ret_val)
	{
		if (0 == InterlockedExchange(&this->release_flag, 1))
		{
			this->request_disconnection(); // 정상 종료.
		}
	}
}

void Session::on_handling_db_task(DBTask* task)
{

}


bool Session::is_valid(uint64_t session_id)
{
	return this->session_id = session_id;
}

