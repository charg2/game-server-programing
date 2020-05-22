#pragma once

#include <cstdint>

namespace c2::enumeration
{
	enum PacketType : char
	{
		PT_NONE,

		C2S_LOGIN = 1,
		C2S_MOVE,
		
		S2C_LOGIN_OK = 1,
		S2C_MOVE,
		S2C_ENTER,
		S2C_LEAVE,

		PT_MAX,
	};
}

#pragma pack(push, 1)

using namespace c2::enumeration;

struct PacketHeader 
{
	char		length;
	PacketType	type;

	static constexpr size_t header_length { sizeof(uint16_t) + sizeof(PacketType) };
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

constexpr unsigned char D_UP = 0;
constexpr unsigned char D_DOWN = 1;
constexpr unsigned char D_LEFT = 2;
constexpr unsigned char D_RIGHT = 3;

struct cs_packet_move {
	char	size;
	char	type;
	char	direction;
};


#pragma pack(pop)
