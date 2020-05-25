#include "../C2Server/C2Server/pre_compile.h"
#include "MMOSession.h"
#include "MMOSimulator.h"
#include "MMOZone.h"
#include "MMOActor.h"
#include "MMOServer.h"
#include "function.h"

MMOActor::MMOActor(MMOSession* owner)
	: x{}, y{},
	name{},
	session_id{},
	current_sector{}, prev_sector{},
	zone{}, simulator{},
	state{ ACS_NONE }, 
	session{ owner }, last_move_time{}
{
}

MMOActor::~MMOActor()
{
}

void MMOActor::enter_sector(int32_t x, int32_t y)
{
	prev_sector = nullptr;

	current_sector = current_sector = zone->get_sector(x, y);

	//current_sector
}

void MMOActor::move_to(int32_t x, int32_t y)
{
	this->x = x; 
	this->y = y;

	MMOSector* target_sector = zone->get_sector(this->y, this->x);

	if (false == target_sector->get_has_obstacle() && this->prev_sector != target_sector) // �̵� �Ҽ� �ִٸ� 
	{
		this->current_sector->leave_actor(session_id);
		target_sector->accept_actor(session_id, this);	// ���͸� �̵���.
		this->current_sector = target_sector;			// ���͸� �̵��ϰ� ���� ���͸� ������.
	}
}

void MMOActor::move(int8_t direction)
{
	
	// ��ֹ� üũ ���.
	switch (direction)
	{
	case c2::constant::D_DOWN: 
		this->y = clamp( 0, this->y +1, 399 );
		this->direction = NEAR_DOWN;
		break;
	case c2::constant::D_LEFT: 
		this->x = clamp(0, this->x -1, 399);
		this->direction = NEAR_LEFT;
		break;
	case c2::constant::D_RIGHT: 
		this->x = clamp(0, this->x + 1, 399);
		this->direction = NEAR_RIGHT;
		break;
	case c2::constant::D_UP:
		this->y = clamp(0, this->y - 1, 399);
		this->direction = NEAR_UP;
		break;

	default:
		return;
	}

	prev_sector = (MMOSector*)'MOVE';
}

void MMOActor::reset()
{
	session_id = this->session->session_id;
	simulator = &MMOSimulator::get_instance();
	zone = nullptr;
	current_exp = 0;
	levelup_exp = 0;
	hp = 10;
	level = 1;
		
	state = ACS_NONE;
	direction = NEAR_MAX;

	x = rand() % c2::constant::MAP_WIDTH;
	y = rand() % c2::constant::MAP_HEIGHT;

	name[0] = NULL;

	current_sector	= nullptr;
	prev_sector		= nullptr;
}




void MMOActor::get_login_packet_info(sc_packet_login_ok& out_payload)
{
	out_payload.header.length = sizeof(sc_packet_login_ok);
	out_payload.header.type = c2::enumeration::S2C_LOGIN_OK;

	out_payload.id = (int16_t)this->session_id;

	out_payload.x = this->x;
	out_payload.y = this->y;
	
	out_payload.hp = this->hp;
	out_payload.exp = this->current_exp;

	out_payload.level = this->level;
}

void MMOActor::simulate()
{
	MMOServer* server = (MMOServer*)session->server;

	// ó�� �α��� ������.. 
	if (prev_sector == nullptr)
	{
		// �� ���� ����ü ���� �ʱ�ȭ 
		sc_packet_enter my_info_payload;
		my_info_payload.header.length = sizeof(sc_packet_enter); 
		my_info_payload.header.type = S2C_ENTER; // header
		my_info_payload.id = (int16_t)session_id;
		memcpy(my_info_payload.name, this->name, 50);
		my_info_payload.x = this->x; my_info_payload.y = this->y;

		//Ÿ�� ����
		sc_packet_enter other_info_payload;
		auto& actors = zone->get_actors();
		other_info_payload.header.length = sizeof(sc_packet_enter);
		other_info_payload.header.type = S2C_ENTER;

		for ( auto& iter : actors )
		{
			MMOActor* other = iter.second;

			c2::Packet*		my_info_packet = c2::Packet::alloc();
			c2::Packet*		other_info_packet = c2::Packet::alloc();

			// Ÿ�� ����
			other_info_payload.id = other->get_id();
			memcpy(other_info_payload.name, other->name, sizeof(sc_packet_enter::name));
			other_info_payload.x = other->get_x();
			other_info_payload.y = other->get_y();
		
			// ������ Ÿ������ ������
			my_info_packet->write(&my_info_payload, sizeof(sc_packet_enter));
			// Ÿ������ ������ ������
			other_info_packet->write(&other_info_payload, sizeof(sc_packet_enter));

			server->send_packet( other->get_session_id(), my_info_packet); // ������ ������ ������
			server->send_packet( this->get_session_id(), other_info_packet); // Ÿ������ ������ ������
		}


		/// ���������� sector �ݿ�.
		prev_sector = (MMOSector*)'NONE';

		return;
	}
	// �̵��Ѱ��;
	else if (prev_sector = (MMOSector*)'MOVE')
	{
		sc_packet_move move_payload;
		move_payload.header.length = sizeof(sc_packet_move);
		move_payload.header.type = S2C_MOVE;
		move_payload.x = this->x;
		move_payload.y = this->y;
		move_payload.id = this->get_id();
		move_payload.move_time = this->last_move_time;
		// move ���� ��.

		auto& all_actors = zone->get_actors();
		for (auto& it : all_actors)
		{
			c2::Packet* out_packet = c2::Packet::alloc();
			out_packet->write(&move_payload, sizeof(sc_packet_move));
			server->send_packet(it.second->get_session_id(), out_packet); // ������ Ÿ������ ������
		}

		/// ���������� sector �ݿ�.
		prev_sector = (MMOSector*)'NONE';
		this->last_move_time = 0; 
		return;
	}
}

void MMOActor::attack()
{
}


uint64_t MMOActor::get_session_id()
{
	return session_id;
}

MMOSector* MMOActor::get_current_sector()
{
	return this->current_sector;
}

MMOSector* MMOActor::get_prev_sector()
{
	return this->prev_sector;
}

int16_t MMOActor::get_id()
{
	return (int16_t)session_id;
}

void MMOActor::set_current_sector(MMOSector* sector)
{
	current_sector = sector;
}

void MMOActor::set_prev_sector(MMOSector* sector)
{
	prev_sector = sector;
}

int32_t MMOActor::get_x()
{
	return x;
}

int32_t MMOActor::get_y()
{
	return y;
}

unsigned MMOActor::get_move_time()
{
	return last_move_time;
}


void MMOActor::set_zone(MMOZone* zone)
{
	this->zone = zone;
}

void MMOActor::set_name(char* name_ptr)
{
	memcpy(this->name, name_ptr, sizeof(cs_packet_login::name));
}

void MMOActor::set_move_time(unsigned time)
{
	this->last_move_time = time;
}

void MMOActor::set_state(c2::enumeration::MMOActorState state)
{
	this->state = state;
}

