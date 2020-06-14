#pragma once

#include "../protocol.h"
#include "Packet.h"
#include "Session.h"


using namespace::c2;
using  HandlerFunc = void (*)(Session* session, PacketHeader header, c2::Packet& in_packet);
extern HandlerFunc handler_table[PT_MAX];
static void default_handler(Session* session, PacketHeader header, c2::Packet& in_packet);
typedef struct InitializeHandlers
{
	InitializeHandlers()
	{
		for (int i = 0; i < PT_MAX; ++i)
			handler_table[i] = default_handler;
	}

} _init_handlers_;

typedef struct RegisterHandler
{
	RegisterHandler(int pt_type, HandlerFunc handler)
	{
		handler_table[pt_type] = handler;
	}
} _register_handlers_;

void default_handler(Session* session, PacketHeader header, c2::Packet& in_packet)
{
	printf_s("Call Default Handler...PT ID: %d\n", header.type);
}

#define REGISTER_HANDLER(PT_TYPE)\
	static void handler_##PT_TYPE(Session* session, PacketHeader header,  c2::Packet& in_packet);\
	static struct RegisterHandler _register_##PT_TYPE(PT_TYPE, handler_##PT_TYPE); \
	static void handler_##PT_TYPE(Session* session, PacketHeader header,  c2::Packet& in_packet)
