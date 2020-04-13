#pragma once
#include "pre_compile.h"

class OuterServer
{
public:
	OuterServer();
	~OuterServer();



protected:
	void io_thread_function();
	void accept_thread_function();

	//static void run(OuterServer* server, c2::enumeration::ThreadType thread_type);


	SOCKET lisetn_sock;
};
