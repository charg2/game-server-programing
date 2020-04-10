#pragma once

#include "pre_compile.h"
#include "configuration.h"
#include "../../Common/OverlappedContext.h"
#include "../../Common/CircularBuffer.hpp"

#include "../../Common/Packet.h"

using namespace c2;

struct PacketHeader;
class OuterServer;
class c2::Packet;


struct Session
{
public:
	Session();
	virtual ~Session();

	bool post_recv();
	void pre_send(c2::Packet* packet);
	void pre_send(PacketHeader* payload);
	void post_send();

	void parse_packet();
	void bind_serer(OuterServer* server);
	void disconnet();

	void increase_reference();
	void decrease_reference();


	//virtual	void on_parse_packet(PacketHeader header, c2::Packet* packet);
	virtual void send_packet(PacketHeader* header);
	
	// packet을 보내고...
	// 처리하는곳에서 
	c2::enumeration::DisconnectReason	get_disconnected_reason();
	void								set_disconnected_reason(c2::enumeration::DisconnectReason dr);
	OuterServer*						get_server();

	// only read after init
	size_t								unique_session_id;	//  8 byte
	OuterServer*						server;				//  8 byte
	SOCKET								socket;				//  8 byte
	SOCKADDR							sock_addr;			// 16 byte
															// 40 byte
	// 
	OverlappedContext 					recv_overlapped;
	OverlappedContext 					send_overlapped;
	CircularBuffer<65536>				recv_buffer;	//
	CircularBuffer<65536>				send_buffer;
	c2::Packet							recv_packet;
	c2::enumeration::DisconnectReason	disconnected_reason;
	size_t								send_flag;
	int32_t								refer_count;

	// for debug
	size_t								total_sent_byte;
	size_t								total_recv_byte;
};


void on_recv_completion_routine(DWORD dwError, DWORD cbTransferred, LPOVERLAPPED lpOverlapped, DWORD dwFlags);
void on_send_completion_routine(DWORD dwError, DWORD cbTransferred, LPOVERLAPPED lpOverlapped, DWORD dwFlags);

