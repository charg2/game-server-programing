#pragma once
//#include "../C2Server/C2Server/OuterServer.h"
class EchoServer : public OuterServer
{
public:
	// OuterServer을(를) 통해 상속됨
	EchoServer();
	virtual ~EchoServer();


	virtual void on_connect(uint64_t session_id) override;
	virtual void on_disconnect(uint64_t session_id) override;
	virtual bool on_accept(Session* session) override;
	virtual void on_wake_io_thread() override;
	virtual void on_sleep_io_thread() override;
	virtual void custom_precedure(uint64_t idx) override;
};

