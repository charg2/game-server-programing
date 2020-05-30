#include "../C2Server/C2Server/pre_compile.h"
#include "MMOSession.h"
#include "MMOSimulator.h"
#include "MMOZone.h"
#include "MMOActor.h"
#include "MMONear.h"
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
{}

MMOActor::~MMOActor()
{}


void MMOActor::move(int8_t direction)
{
	// 장애물 체크 등등.
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
	}

	MMOSector* target_sector = zone->get_sector(this->y, this->x);
	
	if ( false == target_sector->get_has_obstacle() && this->prev_sector != target_sector) // 이동 할수 있다면 
	{
		this->current_sector->leave_actor(session_id);
		target_sector->accept_actor(session_id, this);	// 섹터를 이동함.
		this->current_sector = target_sector;			// 섹터를 이동하고 현재 섹터를 변경함.
	}
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

	//static int n = 0;
	//x = y = n++;

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

	// 처음 로그인 했을때.. 
	if (prev_sector == nullptr)
	{
		//// 내 정보 구조체 생성 초기화 
		//sc_packet_enter my_info_payload;
		//my_info_payload.header.length = sizeof(sc_packet_enter); 
		//my_info_payload.header.type = S2C_ENTER; // header
		//my_info_payload.id = (int16_t)session_id;
		//memcpy(my_info_payload.name, this->name, 50);
		////my_info_payload.o_type; // 아마 오브젝트 타입인듯.
		//my_info_payload.x = this->x; my_info_payload.y = this->y;


		//c2::Packet* my_info_packet = c2::Packet::alloc();
		//my_info_packet->write(&my_info_payload, sizeof(sc_packet_enter));// 내정보 타인한테 보내기



		//// 타인 정보
		//sc_packet_enter other_info_payload;
		//other_info_payload.header.length = sizeof(sc_packet_enter);
		//other_info_payload.header.type = S2C_ENTER;


		////  주변 섹터 구해서 
		//MMONear* near_sectors = current_sector->get_near(this->y, this->x);
		//int cnt = near_sectors->count;
		//for (int n = 0; n < cnt; ++n)
		//{
		//	// 섹터별 인원 구해서
		//	auto near_actors = near_sectors->sectors[n]->actors;

		//	// 주변 인원들에게 내 정보 페킷 만들어서 발사.
		//	for (auto& it : near_actors)
		//	{
		//		MMOActor* other = it.second;

		//		if (this != other) continue;
		//		if (is_near(other) != true) continue;


		//		other_info_payload.id = other->get_id();
		//		memcpy(other_info_payload.name, other->name, sizeof(sc_packet_enter::name));
		//		other_info_payload.x = other->get_x();
		//		other_info_payload.y = other->get_y();


		//		// 타인정보 나한테 보내기
		//		c2::Packet* other_info_packet = c2::Packet::alloc();
		//		other_info_packet->write(&other_info_payload, sizeof(sc_packet_enter));
		//		server->send_packet(this->session_id, other_info_packet); // 타인정보 나한테 보내기

		//		// 내정보 주변에 보내기.
		//		my_info_packet->increase_ref_count();
		//		server->send_packet(other->session_id, my_info_packet); // 내정보 남한테 보내기
		//	}
		//}
		//my_info_packet->decrease_ref_count();

		///// 마지막으로 sector 반영.
		//prev_sector = current_sector;

		//return;
	}
	
	// 이동한경우;
	//else 
		//if (prev_sector != current_sector)
	//{
	//	sc_packet_leave my_leave_payload;
	//	my_leave_payload.header.length = sizeof(sc_packet_leave);
	//	my_leave_payload.header.type = S2C_LEAVE;
	//	my_leave_payload.id = this->get_id();

	//	// leave 보낼 곳.

	//	c2::Packet* my_leave_packet = c2::Packet::alloc();
	//	my_leave_packet->write(&my_leave_payload, sizeof(sc_packet_leave));


	//	MMONear* near_sectors = current_sector->get_near(this->y, this->x);
	//	int cnt = near_sectors->count;
	//	for (int n = 0; n < cnt; ++n)
	//	{
	//		std::map<uint16_t, MMOActor*>& old_actors = old_sector->get_actors();

	//		for (auto& it : old_actors)
	//		{
	//			MMOActor* other = it.second;

	//			sc_packet_leave other_leave_payload;
	//			other_leave_payload.header.length = sizeof(sc_packet_leave);
	//			other_leave_payload.header.type = S2C_LEAVE;
	//			other_leave_payload.id = other->get_id();
	//			
	//			c2::Packet* other_leave_packet = c2::Packet::alloc();
	//			other_leave_packet->write(&other_leave_payload, sizeof(sc_packet_leave));

	//			my_leave_packet->increase_ref_count();
	//			server->send_packet(other->get_session_id(), my_leave_packet); // 타인정보 나한테 보내기
	//			server->send_packet(this->get_session_id(), other_leave_packet); // 타인정보 나한테 보내기
	//		}
	//	}
	//	my_leave_packet->decrease_ref_count();


	//	sc_packet_move move_payload;
	//	move_payload.header.length = sizeof(sc_packet_move);
	//	move_payload.header.type = S2C_MOVE;
	//	move_payload.x = this->x;
	//	move_payload.y = this->y;
	//	move_payload.id = this->get_id();
	//	move_payload.move_time = this->last_move_time;
	//	
	//	c2::Packet* out_packet = c2::Packet::alloc();
	//	out_packet->write(&move_payload, sizeof(sc_packet_move));

	//	// move 보낼 곳.
	//	for (MMOSector* inter_sector : *intersection_sectors)
	//	{
	//		std::map<uint16_t, MMOActor*>& inter_actors = inter_sector->actors;

	//		for (auto& it : inter_actors)
	//		{
	//			out_packet->increase_ref_count();
	//			server->send_packet(it.second->get_session_id(), out_packet); // 타인정보 나한테 보내기
	//		}

	//	}
	//	out_packet->decrease_ref_count();



	//	// 내정보 타인한테 보내기.
	//	sc_packet_enter my_enter_payload;
	//	my_enter_payload.header.length = sizeof(sc_packet_enter);
	//	my_enter_payload.header.type = S2C_ENTER;
	//	my_enter_payload.id = this->get_id();
	//	memcpy(my_enter_payload.name, this->name, sizeof(sc_packet_enter::name));
	//	my_enter_payload.x = x;
	//	my_enter_payload.y = y;

	//	c2::Packet* my_enter_packet = c2::Packet::alloc();
	//	my_enter_packet->write(&my_enter_payload, sizeof(sc_packet_enter));


	//	// enter 보낼 곳.
	//	for (MMOSector* new_sector : *new_difference_sectors)
	//	{
	//		std::map<uint16_t, MMOActor*>& new_actors = new_sector->get_actors();

	//		for (auto& it : new_actors)
	//		{
	//			MMOActor* other = it.second;
	//			sc_packet_enter other_enter_payload;
	//			other_enter_payload.header.length = sizeof(sc_packet_enter);
	//			other_enter_payload.header.type = S2C_ENTER;
	//			other_enter_payload.id = other->get_id();
	//			memcpy(other_enter_payload.name, other->name, sizeof(sc_packet_enter::name));
	//			other_enter_payload.x = other->get_x();
	//			other_enter_payload.y = other->get_y();


	//			my_enter_packet->increase_ref_count();

	//			c2::Packet* other_enter_packet = c2::Packet::alloc();
	//			other_enter_packet->write(&other_enter_payload, sizeof(sc_packet_enter));
	//			server->send_packet(this->get_session_id(), other_enter_packet); // 타인정보 나한테 보내기
	//			server->send_packet(other->get_session_id(), my_enter_packet); // 타인정보 나한테 보내기
	//		}
	//	}
	//
	//	my_enter_packet->decrease_ref_count();
	//	/// 마지막으로 sector 반영.
	//	prev_sector = current_sector;
	//	this->last_move_time = 0; 
	//	return;
	//}
}

void MMOActor::attack()
{
	// 주변 범위를 공격함.
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

bool MMOActor::is_near(MMOActor* actor)
{
	if (abs(this->x - actor->x) > FOV_WIDTH) return false;
	if (abs(this->y - actor->y) > FOV_HALF_HEIGHT) return false;

	return true;
}

