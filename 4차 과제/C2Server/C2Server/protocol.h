#pragma once

#include <cstdint>
#include "enviroment.h"


namespace c2::enumeration
{
	//enum PacketType : uint16_t
	//{
	//	PT_NONE,

	//	PT_CS_HI,
	//	PT_SC_HI,
	//	PT_SC_OTHER_INFO,
	//	PT_SC_HI_AROUND,
	//	
	//	PT_CS_CHATTING,
	//	PT_SC_CHATTING,

	//	PT_CS_MOVE,
	//	PT_SC_MOVE,

	//	PT_CS_BYE,
	//	PT_SC_BYE,

	//	PT_CS_HEARTBEAT,
	//	PT_SC_HEARTBEAT,

	//	PT_SC_ECHO,
	//	PT_CS_ECHO,

	//	PT_MAX,
	//};

	enum PacketType : char
	{
		PT_NONE,

		C2S_LOGIN = 1,
		C2S_MOVE,
		C2S_CHAT,

		S2C_LOGIN_OK = 1,
		S2C_MOVE,
		S2C_ENTER,
		S2C_LEAVE,

		PT_MAX,
	};
}

#pragma pack(push, 1)

using namespace c2::enumeration;
using namespace c2::constant;

struct PacketHeader
{
	char		length;
	PacketType	type;

	static constexpr size_t header_length{ sizeof(uint16_t) + sizeof(PacketType) };
};

static_assert(sizeof(int32_t) == sizeof(int) && sizeof(uint32_t) == sizeof(unsigned int));

struct sc_packet_login_ok
{
	PacketHeader header;

	int		id;
	short	x, y;
	short	hp;
	short	level;
	int		exp;
};

struct sc_packet_move
{
	PacketHeader header;

	int id;
	short x, y;
	unsigned move_time;
};

struct sc_packet_enter
{
	PacketHeader header;

	int id;
	char name[MAX_ID_LEN];
	char o_type;
	short x, y;
};

struct sc_packet_leave
{
	PacketHeader header;

	int id;
};

struct sc_packet_chat
{
	PacketHeader header;

	int	 id;
	char chat[100];
};

struct cs_packet_login
{
	PacketHeader header;

	char	name[MAX_ID_LEN];
};


struct cs_packet_move 
{
	PacketHeader header;

	char		direction;
	unsigned	move_time;
};

struct cs_packet_chat
{
	PacketHeader header;

	int	 id;
	char chat[100];
};

#pragma pack(pop)
