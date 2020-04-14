#pragma once


class OuterServer;
class CircularBuffer;

#define inline_increase_refer(session) 		if ( 0 >= InterlockedIncrement64(&refer_count)) { c2::util::crash_assert(); }
#define inline_decrease_refer(session) 		\
											uint64_t ret_val = InterlockedIncrement64(&refer_count);\
											if (0 >= ret_val)\
											{\
												if (0 == ret_val)\
												{\
													if (0 != InterlockedExchange(&this->release_flag, 1))\
													{ \
														server->disconnect(this); \
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

class Session
{
public:
	Session();
	~Session();

	void post_recv();
	void post_send(c2::Packet* );
	void post_accept();

	void recv_completion();
	void accept_completion();

	void increase_refer();
	void decrease_refer();

private:
	// read and write frequently.
	int64_t			refer_count;	// 8
	int64_t			session_id;		// 8

	CircularBuffer<65536> recv_buffer;			
	// c2::concurrent_queue packet buffer;

	IoContext		recv_context;	// 48 ( 32 + 8 + 8 )
	IoContext		send_context;	// 48 ( 32 + 8 + 8 )
	IoContext		accept_context;	// 48 ( 32 + 8 + 8 )
	IoContext		discon_context;	// 48 ( 32 + 8 + 8 )

	uint64_t		release_flag;	// 8
	SOCKET			sock;			// 8
	OuterServer*	server;			// 8

	friend class OuterServer;
};

