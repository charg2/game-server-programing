#pragma once
#include "Lib/ClientSession.h"


class EchoSession : public ClientSession
{
public:
	EchoSession();
	virtual ~EchoSession();

	virtual	void on_parse_packet(PacketHeader header, c2::Packet* packet) override final;

	void do_tseting_echo();

	//friend class EchoClient;
};

