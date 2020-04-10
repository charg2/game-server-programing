#include "stdafx.h"
#include "EchoSession.h"
#include "EchoClient.h"
#include "../../Common/network_struct.h"
#include "Layer.h"
#include "Player.h"
#include "OtherPlayer.h"

EchoSession::EchoSession() 
{
}

EchoSession::~EchoSession()
{
	
}

void EchoSession::on_parse_packet(PacketHeader header, c2::Packet* packet)
{
	printf("onParsePacket header_type : %d \n\n", header.type);
	using namespace c2::enumeration;
	switch (header.type)
	{
	case PacketType::PT_SC_HI: // ���� ���� ��ȸ�� ���������� ����� ����.
	{
		HiResponse response{};

		packet->read(&response, sizeof(HiResponse));

		printf("PT_SC_HI session_id : %d , x : %d, y : %d\n", response.session_id, response.x, response.y);


		if (false == response.result)
		{
			/// ó�� 
			return;
		}

		wmemcpy(response.nickname, g_player->nickname, sizeof(g_player->nickname) / 2);
		this->unique_session_id = response.session_id;
		g_player->x = response.x;
		g_player->y = response.y;

		return;
	}
	case PacketType::PT_SC_OTHER_INFO: // ���� ���� ��ȸ�� ���������� ����� ����.
	{
		OtherInfo info{};
		packet->read(&info, sizeof(OtherInfo));

		printf("PT_SC_OTHER_INFO session_id : %d , x : %d, y : %d\n", info.session_id, info.x, info.y);

		if (info.session_id == g_echo_client->get_session_id())
		{
			c2::util::crash_assert();
		}

		OtherPlayer* other = OtherPlayer::allocate();

		wmemcpy(other->nickname, info.nickname, sizeof(other->nickname) / 2);
		other->session_id = info.session_id;
		other->x = info.x;
		other->y = info.y;

		if (false == ((EchoClient*)this->client)->others.insert(std::unordered_map<uint64_t, OtherPlayer*>::value_type(info.session_id, other)).second) // ���Խ��� �ߺ��� ����.
		{
			printf("�ߺ��� ����.sessuib_d : %d \n", other->session_id);
			return;
		}

		g_player->get_layer()->add_object(other);
		return;
	}
	case PacketType::PT_SC_HI_AROUND: // �ź� �޾ƶ�
	{
		HiResponseAround response;
		packet->read(&response, sizeof(response));

		printf("PT_SC_HI_AROUND session_id : %d , x : %d, y : %d\n", response.session_id, response.x, response.y);


		OtherPlayer* other = OtherPlayer::allocate();

		wmemcpy(other->nickname, other->nickname, sizeof( other->nickname) /2 );
		other->session_id = response.session_id;
		other->x = response.x;
		other->y = response.y;

		// �߸��� �༮�� ... �α��� �ϸ�ȵǴµ�۸� ű���ؼ� Ŭ�� ������.

		// �ֺ� ������ ������ �޾ƾ���.
		if (false == ((EchoClient*)this->client)->others.insert(std::unordered_map<uint64_t, OtherPlayer*>::value_type(response.session_id, other)).second) // ���Խ��� �ߺ��� ����.
		{
			// �ߺ� ó��.
			return;
		}

		g_player->get_layer()->add_object(other);
		return;
	}
	case PacketType::PT_SC_MOVE:
	{
		MoveResponse response;
		packet->read(&response, sizeof(response));
		printf("PT_SC_MOVE session_id : %d , x : %d, y : %d\n", response.session_id, response.x, response.y);
		if (response.session_id == g_echo_client->get_session_id()) // ����
		{
			g_player->move_using_response(response);
			return;
		}
		else //other
		{
			OtherPlayer* other{};
			if (g_echo_client->try_get_other_player(response.session_id, other))
			{
				other->x = response.x;
				other->y = response.y;
				other->z = response.z;
				return;
			}
			else
			{
				// ���°���.
				printf("���� ���� : %d", response.session_id);
			}
		}
		
		return;
	}
	case PacketType::PT_SC_BYE:
	{
		ByeResponse response;
		packet->read(&response, sizeof(response));
		printf("PT_SC_BYE session_id : %d \n", response.session_id);

		if (response.session_id == g_echo_client->get_session_id()) // ����
		{
			// �� ���� ������...
			disconnect();
			return;
		}

		auto result = ((EchoClient*)this->client)->others.find(response.session_id);
		if (result == ((EchoClient*)this->client)->others.end())
		{
			// not find
		}
		else
		{
			// ���� ó��������Ұ��� �ִ°�... 
			// client ������ ��������.
			g_player->get_layer()->remove_object(result->second);
			size_t is_completed = ((EchoClient*)this->client)->others.erase(response.session_id);
			if (is_completed != 1) // �� ��������
			{

			}

			OtherPlayer::free(result->second);
		}


		break;
	}
	default:
		c2::util::crash_assert();
		return;
	}
}

void EchoSession::do_tseting_echo()
{
	//MoveRequest move_request{};
	//move_request.type = PT_CS_Echo;
	//move_request.length = MoveRequest::packet_length;
	//move_request.direction = 5;
	//move_request.session_id = 56;

	//pre_send(&move_request, MoveRequest::packet_length);

	//
}
