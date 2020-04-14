#include "pre_compile.h"
#include "../../Common/CircularBuffer.hpp"
#include "IOContext.h"
#include "Session.h"
#include "../../Common/exception.h"


void Session::post_recv()
{

	WSABUF		wsa_bufs[2];
	uint32_t	buffer_count;
	size_t		first_space_size = recv_buffer.direct_enqueue_size();
	uint32_t	flag{};

	// 여기부 생길 오류는 nullptr reference or buffer문제...
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

	this->increase_refer();

	int64_t ret_val = WSARecv(sock, wsa_bufs, buffer_count, NULL, (LPDWORD)&flag, &this->recv_context.overalpped, NULL);
	if (SOCKET_ERROR == ret_val)
	{
		uint32_t local_last_error = GetLastError();
		if (WSA_IO_PENDING != local_last_error)
		{
			printf("Not IO PENDING : %d \n", local_last_error);
			
			this->decrease_refer();

			if (WSAECONNRESET == local_last_error)
			{
			}
			else if (ERROR_OPERATION_ABORTED == local_last_error)
			{
			}

			return;
		}
	}

	return;
}

void Session::post_send()
{

}

void Session::recv_completion(size_t transfer_bytes)
{
	OverlappedContext* context_ptr{ reinterpret_cast<OverlappedContext*>(lpOverlapped) };
	Session* session{ reinterpret_cast<Session*>(context_ptr->session) };

	session->recv_buffer.move_front(transfer_bytes);
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

	//this->increase_refer();
	//this->decrease_refer();
	

	session->post_recv();

}

void Session::accept_completion()
{
	

}

void Session::increase_refer()
{
	if ( 0 >= InterlockedIncrement64(&refer_count))
	{
		c2::util::crash_assert();
	}
}

void Session::decrease_refer()
{
	uint64_t ret_val = InterlockedIncrement64(&refer_count);
	if (0 >= ret_val)
	{
		if ( 0 == ret_val)
		{
			if (0 != InterlockedExchange(&this->release_flag, 1))
			{
				server->disconnect(this); // 정상 종료.
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
