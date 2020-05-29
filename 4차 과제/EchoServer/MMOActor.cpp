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
	current_sector{},//, prev_sector{},
	zone{}, 
	session{ owner }, last_move_time{}
{
	InitializeSRWLock(&lock);
}

MMOActor::~MMOActor(){}

void MMOActor::enter_sector(int32_t x, int32_t y)
{
	//prev_sector = nullptr;

	current_sector = &zone->sectors[x][y];

	
}



void MMOActor::move(int8_t direction)
{
	// 장애물 체크 등등.
	switch (direction)
	{
	case c2::constant::D_DOWN: 
		y = clamp( 0, y + 1, 399 );
		this->direction = NEAR_DOWN;
		break;
	case c2::constant::D_LEFT: 
		x = clamp(0, x - 1, 399);
		this->direction = NEAR_LEFT;
		break;
	case c2::constant::D_RIGHT: 
		x = clamp(0, x + 1, 399);
		this->direction = NEAR_RIGHT;
		break;
	case c2::constant::D_UP:
		y = clamp(0, y - 1, 399);
		this->direction = NEAR_UP;
		break;
	}

	MMOSector* target_sector = &this->zone->sectors[y][x];
	if ( false == target_sector->has_obstacle) // 이동 할수 있다면 
	{
		this->current_sector->leave_actor(session_id);
		target_sector->accept_actor(session_id, this);	// 섹터를 이동함.
		this->current_sector = target_sector;			// 섹터를 이동하고 현재 섹터를 변경함.
	}
}

void MMOActor::reset()
{

	session_id = this->session->session_id;
	zone = nullptr;
	current_exp = 0;
	levelup_exp = 0;
	hp = 10;
	level = 1;
	sector_x =  sector_y = 0;
	direction = NEAR_MAX;

	x = rand() % c2::constant::MAP_WIDTH;
	y = rand() % c2::constant::MAP_HEIGHT;

	name[0] = NULL;

	current_sector	= nullptr;

	AcquireSRWLockExclusive(&lock);
	view_list.clear();
	ReleaseSRWLockExclusive(&lock);
}

void MMOActor::exit()
{
}

bool MMOActor::is_near(MMOActor* other)
{
	if (abs(this->x - other->x) > FOV_WIDTH) return false;
	if (abs(this->y - other->y) > FOV_HALF_HEIGHT) return false;

	return true;
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

int16_t MMOActor::get_id()
{
	return int16_t(session_id);
}


void MMOActor::attack()
{
	// 주변 범위를 공격함.
}


