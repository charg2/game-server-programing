#include "pre_compile.h"
#include "../../Common/CircularBuffer.hpp"
#include "../../Common/Packet.hpp"
#include "IOContext.h"
#include "Session.h"
#include "OuterServer.h"
#include "../../Common/exception.h"

using namespace c2::enumeration;

Session::Session() :
io_refer_count{ 0 }, send_flag{ 0 }, packet_sent_count{ 0 },
recv_packet{ }, 
accept_context{ {}, IO_ACCEPT, nullptr},
send_context{ {},  IO_SEND, nullptr }, 
recv_context{ {},  IO_RECV, nullptr }, 
discon_context{ {},IO_DISCONNECT, nullptr },
sock_addr{}, total_recv_bytes{}, total_sent_bytes{},
release_flag{}, io_cancled{}, sock{ INVALID_SOCKET }, server{ nullptr }
{

}

Session::~Session()
{
}

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
			wprintf(L"AcceptEx failed with error: %u\n", WSAGetLastError());
		}

		// �׳ɤ� ����

		// �����ߴٸ� 
		// ������ ���ؼ� reset�ϰ� �ٽ� ���. 

		// ������ ���ÿ� �鰡�� �ٽ� ����.
	}
}

void Session::post_recv()
{
	WSABUF		wsa_bufs[2];
	uint32_t	buffer_count;
	size_t		first_space_size = recv_buffer.direct_enqueue_size();
	uint32_t	flag{};

	// ����� ���� ������ nullptr reference or buffer����...
	this->recv_context.overalpped.Internal = 0;
	this->recv_context.overalpped.InternalHigh = 0;

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

	int64_t ret_val = WSARecv(sock, wsa_bufs, buffer_count, NULL, (LPDWORD)&flag, &this->recv_context.overalpped, NULL);
	if (SOCKET_ERROR == ret_val)
	{
		uint32_t local_last_error = GetLastError();
		if (WSA_IO_PENDING != local_last_error)
		{
			debug_console( printf("Not IO PENDING : %d \n", local_last_error) );
			
			this->decrease_refer();

			if (1 == InterlockedExchange(&this->io_cancled, 1) )
			{
				request_disconnection();
			}
			else
			{
				CancelIoEx(reinterpret_cast<HANDLE>(sock), NULL);
			}

			return;
		}
	}

	return;
}

void Session::request_disconnection()
{
	// session���� �ϰ� 
	if (FALSE == OuterServer::disconnect_ex(sock, &discon_context.overalpped, TF_REUSE_SOCKET, 0))
	{
		DWORD local_last_error = GetLastError();
		if (WSA_IO_PENDING == local_last_error) // ���ش�..
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
		

		// ���� ����...
		// ��Ŷ ������ ����.
		if ( sned_buffer.count() == 0 )
		{
			// send_packet()�� ȣ��Ǹ� post_send()���ΰɷ� ���� PQCS�� ���� ����.
			
			send_flag = 0;
			// �� ���� ���´ٸ�... send_packet() ��Ŷ ������ ����... 

			// ���� ���� send packet�� ȣ��Ǹ� ���������� ������ �ٽ� Ȯ����.
			if( )


			return;
		}

		size_t	send_packet_count = 0;
		WSABUF wsa_bufs[c2::constant::MAX_CONCURRENT_SEND_COUNT];
		DWORD flag{};

		for (; (send_packet_count < c2::constant::MAX_CONCURRENT_SEND_COUNT) && this->sned_buffer.try_pop(this->send_packets[send_packet_count]) ; ++send_packet_count)
		{
			wsa_bufs[send_packet_count].buf = send_packets[send_packet_count].get_buffer();
			wsa_bufs[send_packet_count].len = send_packets[send_packet_count].size();
		}
		

		// ���� ���Ŀ� send�� ������ �ֵ��� ��į?
		send_context.overalpped.Internal = 0;
		send_context.overalpped.InternalHigh = 0;

		this->packet_sent_count = send_packet_count;
		int64_t ret_val = WSASend(this->sock, wsa_bufs, send_packet_count, NULL, flag, &this->send_context.overalpped, NULL);
		if (SOCKET_ERROR == ret_val)
		{
			uint32_t local_last_error = GetLastError();
			if (WSA_IO_PENDING != local_last_error)
			{
				debug_console(printf("Not IO PENDING Send : %d \n", local_last_error));

				this->decrease_refer();

				if (1 == InterlockedExchange(&this->io_cancled, 1))
				{
					request_disconnection();
				}
				else
				{
					CancelIoEx(reinterpret_cast<HANDLE>(sock), NULL);
				}

				return;

			}
		}
	}
}

void Session::send_packet(c2::Packet* out_packet)
{
	// packet�� ����.
	// concurrent_queue�� ����.

	send_buffer.push(out_packet);


	// send���� �ƴ϶��....
	if (send_flag == 0)
	{
		PostQueuedCompletionStatus(server->completion_port, 0, (ULONG_PTR)this, (LPOVERLAPPED)c2::constant::SEND_SIGN);
	}
}


void Session::recv_completion(size_t transfered_bytes)
{
	this->recv_buffer.move_front(transfered_bytes);
	if (0 == this->recv_buffer.get_free_size())
	{
		this->disconnected_reason = c2::enumeration::DisconnectReason::DR_RECV_BUFFER_FULL;

		this->decrease_refer(); 

		return;
	}

	this->parse_packet();

	server->total_recv_bytes[server->local_storage_accessor] += transfered_bytes;

	this->post_recv();
}

void Session::send_completion(size_t transfered_bytes)
{
	server->total_sent_bytes[server->local_storage_accessor] += transfered_bytes;


	uint64_t temp_packet_count = this->packet_sent_count;
	for (size_t n = 0; n < temp_packet_count; ++n)
		Packet::free(packet_buf[n]);



	server->releaseSessionLock(id);
	this->send_flag = 0;

	this->post_send();
}

void Session::accept_completion()
{
	HANDLE returned_hanlde = CreateIoCompletionPort((HANDLE)this->sock, server->completion_port, session_id, 0);
	if(returned_hanlde == NULL ||  returned_hanlde != server->completion_port)
	{
		printf("[ERROR] LanServer::bind() failed");

		session->reset();
		

	}



	// recv �� ref count 
	this->increase_refer();

	if( InterlockedIncrement(&this->io_refer_count);

	this->post_recv();
}

void Session::disconnect_completion()
{
	// io ���� �Ϸ�� ����.
	// �������� �翬;;;
	// ���������� ������ �����ϴ� ���ÿ� ���� �����.
	// session���� �ϰ� 


	decrease_refer();
}

void Session::parse_packet()
{
	using namespace c2::enumeration;

	c2::Packet* local_packet = recv_packet;

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

void Session::increase_refer()
{
	if ( 0 >= InterlockedIncrement64(&io_refer_count))
	{
		this->session_last_error = 0U;
		c2::util::crash_assert();
	}
}

void Session::decrease_refer()
{
	uint64_t ret_val = InterlockedIncrement64(&io_refer_count);
	if (0 >= ret_val)
	{
		if ( 0 == ret_val)
		{
			if (0 != InterlockedExchange(&this->release_flag, 1))
			{
				this->request_disconnection(); // ���� ����.
			}
			else
			{ 
				
			}
		}
		else
		{
			c2::util::crash_assert();
		}
	}
}
