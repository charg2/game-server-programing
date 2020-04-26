#include <string>
//#include "EchoSession.h"
//#include "EchoServer.h"
#include "packet_handler.h"
//#include "math_ex.hpp"

HandlerFunc handler_table[PT_MAX] { };

#define broadcasting_helper( out_data_ptr, session_ptr, except_me ) ((EchoServer*)((EchoSession*)session)->server)->broadcast_to_all(out_data_ptr, session_ptr, except_me);
#define disconnection_helper( target_session, reason ) session->server->disconnect_session(target_session, reason);
#define deserializaton_helper( structor )  in_packet.read(&structor, sizeof(structor));
 
REGISTER_HANDLER(PT_NONE)
{
	printf("\n\nPT_NONE\n\n");
	c2::util::crash_assert();
}

REGISTER_HANDLER(PT_CS_MOVE) 
{
	MoveRequest			request;
	MoveResponse		response;
	Player*				player = ((EchoSession*)session)->player;
	
	//((MoveRequest*)in_packet.get_buffer());
	//deserializaton_helper(request);
	in_packet.read(&request, request.packet_length);
	

	// 세션 아이디 유효성 검사해줘야함.
	if (session->unique_session_id != request.session_id)
	{
		printf("client session id : %d and server session id : %d not machted \n", request.session_id, session->unique_session_id);
	}

	int8_t position_x_before_moving = player->x;
	int8_t position_y_before_moving = player->y;

	switch (request.direction)
	{
	case 37: // left
		player->x -= 1;
		break;

	case 38: // up
		player->y += 1;
		break;

	case 39: // right
		player->x += 1;
		break;

	case 40: // down
		player->y -= 1;
		break;
	default :
		c2::util::crash_assert();
	}

	// clamp
	player->x = my_clamp<int>(-4, player->x, +3);
	player->y = my_clamp<int>(-4, player->y, +3);

	//if (player->x == position_x_before_moving && player->y == position_y_before_moving)
	//{
	//	return;
	//}

	response.type = PT_SC_MOVE;
	response.session_id = session->unique_session_id;
	response.x = player->x;
	response.y = player->y;
	response.z = player->x;

	printf("PT_CS_MOVE session_id : %d, x : %d, y : %d\n", response.session_id, response.x, response.y);

	Sleep(5000);

	broadcasting_helper(&response, session, false);
	// 다 뿌려줌.
}

REGISTER_HANDLER(PT_CS_HI)
{
	printf("PT_CS_HI %d \n", session->unique_session_id);
	HiResponse				response_to_me;		
	HiResponseAround		response_to_other;	
	OtherInfo				info;
	Player*					main_player			{ ((EchoSession*)session)->player };
	

	// 서버에서 부여한 세션 아이디 제공.
	response_to_me.session_id = session->unique_session_id;
	wmemcpy(main_player->nickname, std::wstring{ L"client" + std::to_wstring(response_to_me.session_id) }.c_str(), sizeof(response_to_me.nickname) / 2);
	wmemcpy(response_to_me.nickname, main_player->nickname, sizeof(response_to_me.nickname) / 2);
	response_to_me.x = main_player->x = c2::constant::player_spwan_point_x;
	response_to_me.y = main_player->y = c2::constant::player_spwan_point_y;
	response_to_me.result = true;
	session->pre_send(&response_to_me);

//////////////////////////////////////////////////////////////
	std::vector<Session*> sessions = session->server->get_sessions();
	/*size_t ccu = sessions.size();*/
	for (Session* temp_session : sessions)
	{
		if (temp_session->unique_session_id != session->unique_session_id)
		{
			printf("\n", temp_session, session);

			Player* other_player = ((EchoSession*)temp_session)->player;
			info.x = other_player->x;
			info.y = other_player->y;
			info.session_id = temp_session->unique_session_id;
			wmemcpy(info.nickname, other_player->nickname, sizeof(other_player->nickname) / 2);

			session->pre_send(&info); // 나한테
		}
	}
///////////////////////////////////
	// 주변에 뿌릴 나의 정보.
	wmemcpy(response_to_other.nickname, response_to_me.nickname, sizeof(response_to_me.nickname) / sizeof(wchar_t));
	response_to_other.x = response_to_me.x;
	response_to_other.y = response_to_me.y;
	response_to_other.session_id = response_to_me.session_id;
	
	// 들어온 세션한테.
	broadcasting_helper(&response_to_other, session, true);
	//주변 유저 정보를 뿌려줘야함.
}



REGISTER_HANDLER(PT_CS_BYE)
{
	printf("PT_CS_BYE\n");

	ByeRequest				request;
	ByeResponse				response;

	Player* player			{ ((EchoSession*)session)->player };

	in_packet.read(&request, sizeof(request));
	
	session->pre_send(&response);

	// 서버에서 빼줘야함.
	session->disconnected_reason = c2::enumeration::DR_REQEUSET_FROM_CLIENT;

	session->server->disconnect_session(session);
	//broadcasting_helper(&response, session, false);
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