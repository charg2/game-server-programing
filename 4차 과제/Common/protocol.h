#pragma once

#include <cstdint>

namespace c2::enumeration
{
	enum PacketType : uint16_t
	{
		PT_NONE,

		PT_CS_HI,
		PT_SC_HI,
		PT_SC_OTHER_INFO,
		PT_SC_HI_AROUND,
		
		PT_CS_CHATTING,
		PT_SC_CHATTING,

		PT_CS_MOVE,
		PT_SC_MOVE,

		PT_CS_BYE,
		PT_SC_BYE,

		PT_CS_HEARTBEAT,
		PT_SC_HEARTBEAT,

		PT_SC_ECHO,
		PT_CS_ECHO,

		PT_MAX,
	};

}

#pragma pack(push, 1)

using namespace c2::enumeration;

struct PacketHeader 
{
	uint16_t   length;
	PacketType type;

	static constexpr size_t header_length { sizeof(uint16_t) + sizeof(PacketType) };
};

struct PacketHeaderBase
{
	uint16_t   length;
	PacketType type;

	static constexpr size_t header_length{ sizeof(uint16_t) + sizeof(PacketType) };
};

union PacketHederU
{
	PacketHeaderBase	header;
	int32_t				header_to_i;
};



static_assert(sizeof(PacketHeader) == 4, "\"PacketHeader\" struct length must be 4");
static_assert(sizeof(int32_t) == sizeof(int) && sizeof(uint32_t) == sizeof(unsigned int));

struct HiRequest : public PacketHeader
{
	//fixed

	struct HiRequest()
	{
		length = sizeof(HiRequest); // + 
		type = PT_CS_HI;
	}
};

struct HiResponse : public PacketHeader
{
	wchar_t		nickname[16];
	uint64_t	session_id;
	int8_t		x;
	int8_t		y;
	bool		result;
	uint32_t	reserve;
	HiResponse()
	{
		length = sizeof(HiResponse);
		type = PT_SC_HI;
		reserve = 0xDEADDEAD;
	}
};

struct OtherInfo : public PacketHeader
{
	wchar_t		nickname[16];
	uint64_t	session_id;
	int8_t		x;
	int8_t		y;
	uint32_t	reserve;

	OtherInfo()
	{
		length = sizeof(OtherInfo);
		type = PT_SC_OTHER_INFO;
		reserve = 0xDEADBEEF;
	}
};

struct HiResponseAround : public PacketHeader
{
	uint64_t	session_id;
	wchar_t		nickname[16];
	int8_t		x;
	int8_t		y;

	HiResponseAround()
	{
		length = sizeof(HiResponseAround);
		type = PT_SC_HI_AROUND;
	}
};

struct MoveRequest : public PacketHeader
{
	uint64_t	session_id;
	int8_t		direction;

	static constexpr size_t packet_length{ header_length + sizeof(session_id) + sizeof(direction) };

	MoveRequest()
	{
		length = sizeof(MoveRequest);
		type = PT_CS_MOVE;
	}
};

struct MoveResponse : public PacketHeader
{
	uint64_t		session_id;
	int8_t			x;
	int8_t			y;
	int8_t			z;

	MoveResponse()
	{
		length = sizeof(MoveResponse);
		type = PT_SC_MOVE;
	}
};

struct ByeRequest : public PacketHeader
{
	uint64_t	session_id;

	struct ByeRequest()
	{
		length = sizeof(ByeRequest);
		type = PT_CS_BYE;
	}
};

struct ByeResponse : public PacketHeader
{
	uint64_t	session_id;

	ByeResponse()
	{
		length = sizeof(ByeResponse);
		type = PT_SC_BYE ;
	}
};
#pragma pack(pop)
