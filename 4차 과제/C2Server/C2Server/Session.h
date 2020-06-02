#pragma once
#include "IOContext.h"
#include "concurrency/ConcurrentQueue.h"
#include "CircularBuffer.h"

class OuterServer;
class CircularBuffer;

#define inline_increase_refer(session) 		if ( 0 >= InterlockedIncrement64(&session->refer_count)) { c2::util::crash_assert(); }
#define inline_decrease_refer(session, reason) 		\
											uint64_t ret_val = InterlockedDecrement64(&session->refer_count);\
											if (0 >= ret_val)\
											{\
												if (0 == ret_val)\
												{\
													if (0 != InterlockedExchange(&session->release_flag, 1))\
													{ \
														session->server->request_disconnection(session->session_id, reason); \
													}\
													else\
													{\
														;\
													}\
												}\
												else\
												{\
													c2::util::crash_assert();\
												}\
											}\


// post_recv -> +1 
// recv_completion -> -1
// session_id �������� ����.
// ���� �Ҹ�� ����ī��Ʈ����.
// ref count �ø���... 
/*
1. �������� ����
2. recv ���۽�....  // ó������ �ø����� �׻� recv�����Ұϱ� ������.
3. send  // ������ ������?
4. acceptEx��
5. disconnectEx�� 

// ���� ��û ������
	cancle io ex�� ���ؼ� ���� ���� ��Ŵ.
// disconnectEx �׳� ���� ������.
	
*/

class Session
{
public:
	Session();
	~Session();

	void post_accept();
	void post_recv();
	void request_disconnection();
	void post_send();

	void recv_completion(size_t transfer_bytes);
	void send_completion(size_t transfer_bytes);
	void accept_completion();
	void disconnect_completion();

	void parse_packet();
	//void parse_packet_echo();
	void reset();

	void increase_refer();
	void decrease_refer();

	bool is_valid(uint64_t session_id);

public:
	// read and write frequently.
	int64_t			refer_count;	// 8
	int64_t			session_id;		// 8
	int64_t			send_flag;		// 8
	CircularBuffer  recv_buffer;	// 24
	c2::Packet*		sent_packets[c2::constant::MAX_CONCURRENT_SEND_COUNT]; //   
	c2::Packet		recv_packet;

	uint64_t						packet_sent_count;
	c2::concurrency::ConcurrentQueue<c2::Packet*>	send_buffer;

	IoContext		recv_context;			// 48 ( 32 + 8 + 8 )
	IoContext		send_context;			// 48 ( 32 + 8 + 8 )
	IoContext		accept_context;			// 48 ( 32 + 8 + 8 )
	IoContext		discon_context;			// 48 ( 32 + 8 + 8 )

	SOCKADDR_IN		sock_addr;				// 16
	size_t			total_recv_bytes;		// 8
	size_t			total_sent_bytes;		// 8

	uint64_t		release_flag;			// 8
	uint64_t		io_cancled;
	uint64_t		last_error;
	SOCKET			sock;					// 8
	OuterServer*	server;					// 8

	DisconnectReason disconnected_reason;
	friend class OuterServer;
};

