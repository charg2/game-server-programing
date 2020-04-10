#pragma once

#include "Lib/Session.h"
#include "../Common/protocol.h"
#include "Player.h"

struct PacketHeader;

class EchoSession : public Session
{
public:
	EchoSession();
	virtual ~EchoSession();

	void reset();

	Player* player;
};

