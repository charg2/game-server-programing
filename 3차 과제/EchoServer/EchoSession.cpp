#include "EchoSession.h"



EchoSession::EchoSession()
{
	player = new Player;
	player->x = 0;
	player->y = 0;
}

EchoSession::~EchoSession()
{
	delete player;
}

void EchoSession::reset()
{
	if (nullptr != player)
		delete player;

	player = new Player;
	player->x = 0;
	player->y = 0;
}

//void EchoSession::on_parse_packet(PacketHeader header, c2::Packet* packet)
//{
//	switch (header.type)
//	{
//		case c2::enumeration::PacketType::PT_CS_MOVE:
//		{
//			printf("PT_CS_MOVE");
//			MoveRequest request;
//			MoveResponse response;
//
//			packet->read(&request, request.packet_length);
//
//			switch (request.direction)
//			{
//			case 37: // left
//				player->x -= 1;
//				break;
//
//			case 38: // up
//				player->y += 1;
//				break;
//
//			case 39: // right
//				player->x += 1;
//				break;
//
//			case 40: // down
//				player->y -= 1;
//				break;
//			default :
//				c2::util::crash_assert();
//			}
//
//			// clamp
//			player->y = Clamp<int>(-4, player->y, +3);
//			player->x = Clamp<int>(-4, player->x, +3);
//
//			response.x = player->x;
//			response.y = player->y;
//
//			pre_send(&response, response.packet_length);
//		}
//		return;
//
//		default:
//			c2::util::crash_assert();
//	}
//}

