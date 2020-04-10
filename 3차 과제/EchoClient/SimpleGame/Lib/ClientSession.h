#pragma once

#include "configuration.h"
#include "pre_compile.h"
#include "../../../Common/OverlappedContext.h"
#include "PacketVector.h"

#include "../../../Common/Packet.h"

using namespace c2;

struct PacketHeader;
class OuterClient;
class Packet;
class SocketAddress;
// send 1

class ClientSession
{
public:
	ClientSession();
	virtual ~ClientSession();

	void connect_using_sockaddress(SocketAddress& address);
	void disconnect();
	virtual	void on_parse_packet(PacketHeader header, c2::Packet* packet);
	void init(bool enabled_non_blocked_io = true);
	void apply_sock_opt();

	size_t pre_send(const c2::Packet* out_packet);
	size_t pre_send(PacketHeader* payload);

	void recv_payload();
	void send_payload();

	void bind_client(OuterClient* client);

	uint64_t get_session_id();

	SOCKET get_socket();
protected:
	//bool post_recv();
	//void pre_send(c2::Packet* packet);
	//void pre_send(Header* packet, size_t size);
	//void post_send();
	void parse_packet();
	void disconnet();

	virtual void send_packet(PacketHeader* header);

	// packet을 보내고...
	// 처리하는곳에서 
	c2::enumeration::DisconnectReason	get_disconnected_reason();
	void								set_disconnected_reason(c2::enumeration::DisconnectReason dr);
private:

protected:
	//OverlappedContext 					recv_overlapped;
	//OverlappedContext 					send_overlapped;
	PacketVector<65536>					recv_buffer;	//
	PacketVector<65536>					send_buffer;
	c2::Packet							recv_packet;
	c2::enumeration::DisconnectReason	disconnected_reason;
	size_t								send_flag;
	c2::enumeration::SessionState		state;


	// read only after init;
	size_t								unique_session_id;	//  8 byte
	OuterClient*						client;				//  8 byte
	SOCKET								socket;				//  8 byte
	SOCKADDR							sock_addr;			// 16 byte
															// total 40 byte
};

