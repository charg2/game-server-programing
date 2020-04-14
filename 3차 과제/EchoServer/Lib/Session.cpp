
#include "../../Common/protocol.h"
#include "Session.h"
#include "../packet_handler.h"
#include "OuterServer.h"
#include "../../Common/Packet.hpp"

Session::Session()
: recv_overlapped{ {}, this }
, send_overlapped{ {}, this }
, unique_session_id{}
, socket{ INVALID_SOCKET }
//, release_flag{}
, sock_addr{}
, server{}
, send_flag{}
, total_sent_byte{}
, total_recv_byte{}
{
}

Session::~Session()
{
}

bool Session::post_recv()
{
	WSABUF		wsa_bufs[2];
	uint32_t	buffer_count;
	size_t		first_space_size = recv_buffer.direct_enqueue_size();
	uint32_t	flag{};
	//uint32_t	recv_bytes{};

	this->recv_overlapped.overalpped.Internal = 0;
	this->recv_overlapped.overalpped.InternalHigh = 0;

	if (first_space_size < recv_buffer.get_free_size())
	{
		// buffer 두개인 경우.
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

	int64_t ret_val = WSARecv(socket, wsa_bufs, buffer_count, NULL, (LPDWORD)&flag, &this->recv_overlapped.overalpped, on_recv_completion_routine);
	if (SOCKET_ERROR == ret_val)
	{
		uint32_t local_last_error = GetLastError();
		if (WSA_IO_PENDING != local_last_error)
		{
			printf("Not IO PENDING : %d \n", local_last_error);
			this->decrease_reference();
			if (WSAECONNRESET == local_last_error)
			{
			}
			else if (ERROR_OPERATION_ABORTED == local_last_error)
			{
			}

			return false;
		}
	}

	return true;
}


// 버퍼에 복사만...
void Session::pre_send(PacketHeader* packet)
{
	size_t enqueued_size = this->send_buffer.enqueue((char*)packet, packet->length);
	if( enqueued_size < packet->length)
	{
		// 버퍼 풀...
		//this->custom_error_code = c2::enumeration::ErrorCode::Error_Duplicated_Session_Key;
		this->disconnected_reason = c2::enumeration::DisconnectReason::DR_FROM_SERVER;


		c2::util::crash_assert();
	}
}

// 버퍼에 복사만...
void Session::pre_send(c2::Packet* out_packet)
{
	size_t packet_size = out_packet->size();
	size_t enqueued_size = this->send_buffer.enqueue(out_packet->get_buffer(), packet_size);
	if (enqueued_size < packet_size)
	{
		// 버퍼 풀...
		//this->custom_error_code = c2::enumeration::ErrorCode::Error_Duplicated_Session_Key;
		this->disconnected_reason = c2::enumeration::DisconnectReason::DR_FROM_SERVER;

		c2::util::crash_assert();
	}
}

void Session::post_send()
{
	size_t used_size = this->send_buffer.get_use_size();
	if (used_size <= 0)
	{
		decrease_reference();

		return;
	}

	send_flag = true; // 

	WSABUF		wsa_bufs[2];
	uint32_t	buffer_count;
	size_t		first_space_size = send_buffer.direct_dequeue_size();
	uint32_t	flag{};
	//uint32_t	sent_bytes{}; // for async send

	this->send_overlapped.overalpped.Internal = 0;
	this->send_overlapped.overalpped.InternalHigh = 0;

	if (first_space_size < used_size)
	{
		// buffer 두개인 경우.
		wsa_bufs[0].buf = send_buffer.get_rear_buffer();
		wsa_bufs[0].len = first_space_size;

		wsa_bufs[1].buf = send_buffer.get_buffer();
		wsa_bufs[1].len = used_size - first_space_size;

		buffer_count = 2;
	}
	else
	{
		wsa_bufs[0].buf = send_buffer.get_rear_buffer();
		wsa_bufs[0].len = first_space_size;

		buffer_count = 1;
	}


	if (SOCKET_ERROR == WSASend(socket, wsa_bufs, buffer_count, NULL, flag, &this->send_overlapped.overalpped, on_send_completion_routine))
	{
		uint32_t local_last_error = GetLastError();
		if (WSA_IO_PENDING != local_last_error)
		{
			printf("Not IO PENDING : %d \n", local_last_error);
			if (WSAECONNRESET == local_last_error)
			{
				this->decrease_reference();
				//server->disconnect_session(this, c2::enumeration::DR_FROM_CLIENT);
			}
			else if (ERROR_OPERATION_ABORTED == local_last_error)
			{
				this->decrease_reference();
			}
		}
	}

}

void Session::parse_packet()
{
	using namespace c2::enumeration;

	c2::Packet* local_packet = &recv_packet;
	
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

//
void Session::on_parse_packet(PacketHeader header, c2::Packet* packet)
{
	using namespace c2::enumeration;

	switch (header.type)
	{
	case PT_NONE:
		c2::util::crash_assert();
		break;

	case PT_CS_ECHO:
		printf("CS_Echo \n");
		break;

	case PT_SC_ECHO:
		printf("SC_Echo \n");
		break;

	default:
		c2::util::crash_assert();
		break;
	}
}

void Session::send_packet(PacketHeader* header)
{
}

c2::enumeration::DisconnectReason Session::get_disconnected_reason()
{
	return this->disconnected_reason;
}

void Session::set_disconnected_reason(c2::enumeration::DisconnectReason dr)
{
	this->disconnected_reason = dr;
}

OuterServer* Session::get_server()
{
	return server;
}

void Session::bind_serer(OuterServer* server)
{
	this->server = server;
}

void Session::disconnet()
{
	server->disconnect_session(this);//
}

void Session::increase_reference()
{
	refer_count += 1;
	if (0 >= refer_count )
	{
		c2::util::crash_assert();
	}
}


void Session::decrease_reference()
{
	refer_count -= 1;
	if (0 >= refer_count )
	{
		if (0 > refer_count)
		{
			c2::util::crash_assert();
		}
		else 
		{
			server->disconnect_session(this); 
		}
	}
}


void on_send_completion_routine(DWORD dwError, DWORD cbTransferred, LPOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	OverlappedContext*	context_ptr			{ reinterpret_cast<OverlappedContext*>(lpOverlapped) };
	Session*			session				{ reinterpret_cast<Session*>(context_ptr->session) };
	OuterServer*		server				{ session->server };

	session->send_buffer.move_rear(cbTransferred);
	
	session->total_sent_byte += cbTransferred;

	session->decrease_reference();

	session->send_flag = false;
}


void on_recv_completion_routine(DWORD dwError, DWORD cbTransferred, LPOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	OverlappedContext*	context_ptr		{ reinterpret_cast<OverlappedContext*>(lpOverlapped) };
	Session*			session			{ reinterpret_cast<Session*>(context_ptr->session) };

	session->recv_buffer.move_front(cbTransferred);
	if (0 == session->recv_buffer.get_free_size())
	{
		// recv buffer가 가득 찼다는건 비정상적인 상황 임. 연결끊기
		// log

		session->disconnected_reason = c2::enumeration::DisconnectReason::DR_RECV_BUFFER_FULL;
		
		session->decrease_reference();
		
		return;
	}

	session->parse_packet();
	
	session->total_recv_byte += cbTransferred;
	
	session->increase_reference();
	// 처리 끝.
	session->decrease_reference();

	session->post_recv();
}
