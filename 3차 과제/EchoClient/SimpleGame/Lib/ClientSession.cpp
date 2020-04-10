#include "../../../Common/protocol.h"
#include "ClientSession.h"
#include "OuterClient.h"
#include "../../../Common/SocketAddress.h"
#include "../../../Common/Packet.hpp"

ClientSession::ClientSession() :
	//  recv_overlapped{ {}, this }, send_overlapped{ {}, this }, 
	unique_session_id{}
	, socket{ INVALID_SOCKET }
	//, release_flag{}
	, sock_addr{}
	, client{}
	, send_flag{}
{}

ClientSession::~ClientSession()
{}



void ClientSession::connect_using_sockaddress(SocketAddress& address)
{
	this->state = c2::enumeration::SS_Connecting;

	size_t reconnect_cnt = 0;
	for (;;)
	{
		if (SOCKET_ERROR == ::connect(this->socket, address.get_as_sockaddr(), address.size()))
		{
			DWORD err_code = GetLastError();
			if (WSAEWOULDBLOCK == err_code)
			{
				printf("Is_Connecting.... in ClientSession::connect()");
				client->set_error_code(c2::enumeration::ErrorCode::Error_Socket_Is_Connecting);
				Sleep(10);
				++reconnect_cnt;
				if (reconnect_cnt > 100)
				{
					printf("재접속 시도 100회... 가망이 없음....");
					return;
				}

				continue;
			}
			else if (WSAEISCONN == err_code)
			{
				this->state = c2::enumeration::SS_Connected;
				printf("established");
				return;
			}
			else
			{
				printf("connection failure in ClientSession::connect()\n");
				client->set_error_code(c2::enumeration::ErrorCode::Error_Client_Socket_Connection_Failure);
				//this->state = c2::enumeration::SS_Connecting;
			}
		}
		else
		{
			this->state = c2::enumeration::SS_Connected;
			printf("established");
		}
	}
}

void ClientSession::disconnect()
{
	::closesocket(socket);
}

void ClientSession::on_parse_packet(PacketHeader header, c2::Packet* packet)
{
	switch (1)
	{
		return;
	}
}

void ClientSession::init(bool enabled_non_blocked_io /* = true */)
{
	this->socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == socket)
	{
		printf("socket creatrion failure IN ClientSession::init()");
		client->set_error_code(c2::enumeration::ErrorCode::Error_Socket_Creation_Failure);
		c2::util::crash_assert();
	}

	apply_sock_opt();
}

void ClientSession::apply_sock_opt()
{
	// nagle algo
	BOOL optval = TRUE; // Nagle 알고리즘 중지
	if (SOCKET_ERROR == setsockopt(this->socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval)))
	{
		printf("socket creatrion failure IN ClientSession::init()");
		client->set_error_code(c2::enumeration::ErrorCode::Error_Failure_To_Apply_NoDelay_Opt);
		c2::util::crash_assert();
	}


	// linger opt
	linger ling_optval{ 1, 0 };
	if (SOCKET_ERROR == setsockopt(this->socket, SOL_SOCKET, SO_LINGER, (char*)&ling_optval, sizeof(ling_optval)))
	{
		printf("socket creatrion failure IN ClientSession::init()");
		client->set_error_code(c2::enumeration::ErrorCode::Error_Failure_To_Apply_NoDelay_Opt);
		c2::util::crash_assert();
	}


	// 넌블로킹 소켓으로 전환
	u_long on{ 1 };
	if (SOCKET_ERROR == ioctlsocket(this->socket, FIONBIO, &on))
	{
		printf("socket creatrion failure IN ClientSession::init()");
		client->set_error_code(c2::enumeration::ErrorCode::Error_Failure_To_Apply_None_Block_Opt);
		c2::util::crash_assert();
	}

	BOOL bEnable{ TRUE };
	if (SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bEnable, sizeof(bEnable)))
	{
		printf("socket creatrion failure IN ClientSession::init()");
		client->set_error_code(c2::enumeration::ErrorCode::Error_Failure_To_Apply_KeepAlive_Opt);
		c2::util::crash_assert();
	}
}


size_t ClientSession::pre_send(const c2::Packet* out_packet)
{
	size_t remaining_size = send_buffer.get_free_size(); //  남은 버퍼사이즈
	size_t packet_size = out_packet->size();

	if (remaining_size < packet_size)
		c2::util::crash_assert();

	this->send_buffer.write(out_packet->get_buffer(), packet_size);

	return packet_size;
}

size_t ClientSession::pre_send(PacketHeader* payload)
{
	size_t remaining_size = send_buffer.get_free_size(); //  남은 버퍼사이즈
	size_t packet_size = payload->length;

	if (remaining_size < packet_size)
		c2::util::crash_assert();

	this->send_buffer.write(payload, packet_size);

	return packet_size;
}


void ClientSession::parse_packet()
{
	using namespace c2::enumeration;

	static c2::Packet local_packet;

	for (;;)
	{
		size_t use_size = recv_buffer.get_use_size();
		// recv_buffer에서 하나씩 꺼냄.
		if (use_size < sizeof(PacketHeader))
			break;

		PacketHeader header{};
		if (sizeof(PacketHeader) > recv_buffer.peek(&header, sizeof(PacketHeader)))
			break;

		if (header.length > use_size)
			break;

		// recv_buffer.
		if (PT_NONE < header.type && header.type >= PT_MAX)
			break;

		local_packet.write(recv_buffer.get_read_buffer(), header.length);

		this->on_parse_packet(header, &local_packet);

		local_packet.reset();

		this->recv_buffer.move_read_head(header.length);
	}

	recv_buffer.rewind();
}

void ClientSession::bind_client(OuterClient* client)
{
	this->client = client;
}

uint64_t ClientSession::get_session_id()
{
	return unique_session_id;
}

SOCKET ClientSession::get_socket()
{
	return this->socket;
}


void ClientSession::disconnet()
{
	closesocket(this->socket);

	this->state = c2::enumeration::SS_Disconnected;

	this->disconnected_reason = c2::enumeration::DR_FromClient;
}

void ClientSession::send_packet(PacketHeader* header)
{
}

c2::enumeration::DisconnectReason ClientSession::get_disconnected_reason()
{
	return this->disconnected_reason;
}

void ClientSession::set_disconnected_reason(c2::enumeration::DisconnectReason dr)
{
	this->disconnected_reason = dr;
}

void ClientSession::recv_payload()
{
	int received_bytes = recv(this->socket, (char*)recv_buffer.get_write_buffer(), recv_buffer.get_free_size(), NULL);
	if (SOCKET_ERROR == received_bytes || 0 == received_bytes) // 종료 
	{
		if (WSAEWOULDBLOCK != WSAGetLastError())
		{
			client->set_error_code(c2::enumeration::ErrorCode::Error_None_WOULD_BLOCK);

			disconnect();
		}
	}
	else if (0 < received_bytes)
	{
		recv_buffer.move_write_head(received_bytes);

		this->parse_packet(); // 패킷 처리. // 분리하는게 더 나은 디자인인가...
	}

	return;
}

void ClientSession::send_payload()
{
	if (this->send_buffer.get_use_size() <= 0)
		return;

	int64_t sent_bytes = send(this->socket, this->send_buffer.get_buffer(), send_buffer.get_use_size(), NULL);

	send_buffer.move_read_head(sent_bytes);

	send_buffer.rewind();

	return;
}

