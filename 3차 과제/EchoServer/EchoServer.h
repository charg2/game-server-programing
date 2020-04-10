#pragma once

#include "Lib/OuterServer.h"

class EchoServer : public OuterServer
{
public:
	EchoServer();
	virtual ~EchoServer();

	virtual bool on_initialize_after_init();
	virtual bool on_initialize_before_init();
	virtual void on_connect(Session* session);
	virtual void on_disconnect(Session* session);
	virtual bool on_accept(Session* session);
	virtual void on_finalize_after_fin();
	virtual void on_finalize_before_fin();

	virtual void on_update();

	void broadcast_to_all(c2::Packet* out_packet, Session* caster, bool except_me = false);
	void broadcast_to_all(PacketHeader* out_payload, Session* caster, bool except_me = false);

	void broadcast_around(c2::Packet* out_packet, Session* caster);
	void broadcast_around(PacketHeader* out_payload, Session* caster);

	virtual Session* create_session();
};

