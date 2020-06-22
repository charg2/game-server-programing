#include "pre_compile.h"
#include "MMOSession.h"
#include "MMOZone.h"
#include "MMOActor.h"
#include "util/TimeScheduler.h"
#include "MMOServer.h"
#include "MMONpc.h"
#include "MMODBTask.h"
#include <unordered_set>

MMOActor::MMOActor(MMOSession* owner)
	: x{}, y{},
	name{},
	session_id{},
	//current_sector{},//, prev_sector{},
	zone{}, 
	session{ owner }, last_move_time{}
{
	InitializeSRWLock(&lock);
}

MMOActor::~MMOActor(){}

void MMOActor::enter_sector(int32_t x, int32_t y)
{
	//prev_sector = nullptr;

	//current_sector/* = &zone->sectors[x][y];*/
}


void MMOActor::reset()
{
	AcquireSRWLockExclusive(&lock);
	
	session_id = this->session->session_id;
	zone = nullptr;
	current_exp = 0;
	levelup_exp = 0;
	hp = 200;
	level = 1;

	x = 0;
	y = 0;

	name[0] = NULL;

	view_list.clear();
	view_list_for_npc.clear();

	status = ST_ACTIVE;

	ReleaseSRWLockExclusive(&lock);
}

void MMOActor::exit()
{
}

void MMOActor::reset_data(const LoadActorTask* task)
{
	AcquireSRWLockExclusive(&lock);

	session_id = task->session_id;
	user_id = task->user_id;
	zone = nullptr;

	current_exp = task->exp;
	levelup_exp = task->level * 200;
	hp = task->hp;
	level = task->level;

	x = task->x;
	y = task->y;

	memcpy(this->name, task->name, 50);

	view_list.clear();
	view_list_for_npc.clear();

	status = ST_ACTIVE;

	ReleaseSRWLockExclusive(&lock);
}

void MMOActor::reset_data_when_creation(const CreateActorTask* task)
{
	AcquireSRWLockExclusive(&lock);

	session_id = task->session_id;
	user_id = task->user_id;
	zone = nullptr;

	current_exp = 0;
	levelup_exp = 200;
	hp = 200;
	level = 1;

	x = fast_rand() % 800;
	y = fast_rand() % 800;

	memcpy(this->name, task->name, 50);

	view_list.clear();
	view_list_for_npc.clear();

	status = ST_ACTIVE;

	ReleaseSRWLockExclusive(&lock);
}


bool MMOActor::is_near(MMOActor* other)
{
	if (abs(this->x - other->x) > FOV_HALF_WIDTH) return false;
	if (abs(this->y - other->y) > FOV_HALF_HEIGHT) return false;

	return true;
}

bool MMOActor::is_near(MMONpc* other)
{
	if (abs(this->x - other->x) > FOV_HALF_WIDTH) return false;
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

// 내 뷰리스트에 상대를 추가하고, 정보도 보냄. 
void MMOActor::send_enter_packet(MMOActor* other)
{
	sc_packet_enter payload;
	payload.id = other->get_id();
	payload.header.length = sizeof(sc_packet_enter);
	payload.header.type = S2C_ENTER;
	payload.x = other->x;
	payload.y = other->y;
	wcscpy_s(payload.name, other->name);
	payload.o_type = 0;

	AcquireSRWLockExclusive(&this->lock);
	this->view_list.emplace(other->get_id(), other);
	ReleaseSRWLockExclusive(&this->lock);

	c2::Packet* enter_packet = c2::Packet::alloc();
	enter_packet->write(&payload, sizeof(sc_packet_enter));

	server->send_packet(this->session_id, enter_packet);

	//printf("send_enter_packet() my id : %llu  other id : %llu  \n", this->session_id, payload.id);
}

// 내 뷰리스트에 상대를 추가과정 생략., 정보도 보냄. 
void MMOActor::send_enter_packet_without_updating_viewlist(MMOActor* other)
{
	sc_packet_enter payload;
	payload.id = other->get_id();
	payload.header.length = sizeof(sc_packet_enter);
	payload.header.type = S2C_ENTER;
	payload.x = other->x;
	payload.y = other->y;
	wcscpy_s(payload.name, other->name);
	payload.o_type = 0;


	c2::Packet* enter_packet = c2::Packet::alloc();
	enter_packet->write(&payload, sizeof(sc_packet_enter));

	server->send_packet(this->session_id, enter_packet);

	//printf("send_enter_packet() my id : %llu  other id : %llu  \n", this->session_id, payload.id);
}


void MMOActor::send_enter_packet(MMOActor* other, c2::Packet* enter_packet)
{
	AcquireSRWLockExclusive(&this->lock);
	this->view_list.emplace(other->get_id(), other);
	ReleaseSRWLockExclusive(&this->lock);

	enter_packet->increase_ref_count();
	server->send_packet(this->session_id, enter_packet);

	//printf("send_enter_packet() my id : %llu  other id : %llu  \n", this->session_id, payload.id);
}


// 내 뷰리스트에 상대를 추가하고, 정보도 보냄. 
void MMOActor::send_enter_packet(MMONpc* other)
{
	sc_packet_enter payload;
	payload.id = (int)other->id;
	payload.header.length = sizeof(sc_packet_enter);
	payload.header.type = S2C_ENTER;
	payload.x = other->x;
	payload.y = other->y;
	wcscpy_s(payload.name, other->name);

	payload.o_type = 1;

	AcquireSRWLockExclusive(&this->lock);
	this->view_list_for_npc.emplace((int)other->id);
	ReleaseSRWLockExclusive(&this->lock);

	c2::Packet* enter_packet = c2::Packet::alloc();
	enter_packet->write(&payload, sizeof(sc_packet_enter));

	g_server->send_packet(this->session_id, enter_packet);
}
// 내 정보만 보냄.. 
void MMOActor::send_enter_packet_without_updating_viewlist(MMONpc* other)
{
	sc_packet_enter payload;
	payload.id = (int)other->id;
	payload.header.length = sizeof(sc_packet_enter);
	payload.header.type = S2C_ENTER;
	payload.x = other->x;
	payload.y = other->y;
	wcscpy_s(payload.name, other->name);
	payload.o_type = 1;

	c2::Packet* enter_packet = c2::Packet::alloc();
	enter_packet->write(&payload, sizeof(sc_packet_enter));

	g_server->send_packet(this->session_id, enter_packet);
}



void MMOActor::send_login_ok_packet()
{
	sc_packet_login_ok p;

	this->get_login_packet_info(p);

	c2::Packet* login_ok_packet = c2::Packet::alloc();

	login_ok_packet->write(&p, sizeof(sc_packet_login_ok));

	server->send_packet(this->session_id, login_ok_packet);
	//printf("send_login_ok_packet() id : %llu \n", this->session_id);
}

void MMOActor::wake_up_npc(MMONpc* npc)
{
	if (npc->is_active == NPC_SLEEP) // false
	{
		if (NPC_SLEEP == InterlockedExchange(&npc->is_active, NPC_WORKING) )		// 이전 상태가 자고 있었다면 꺠움.
		{
			// 내가 꺠운 상태.
			// 내가 책임지고 일을 시켜야 함.
			local_timer->push_timer_task(npc->id, TTT_MOVE_NPC, 1000, session_id);
		}
		else 
		{
			return;
		}
	}
}




void MMOActor::send_move_packet(MMOActor* other)
{
	sc_packet_move payload;
	payload.header.length = sizeof(sc_packet_move);
	payload.header.type = S2C_MOVE;

	payload.id			= other->get_id();
	payload.x			= other->x;
	payload.y			= other->y;
	payload.move_time	= other->last_move_time;
	
	c2::Packet* move_packet =  c2::Packet::alloc();

	move_packet->write(&payload, sizeof(sc_packet_move));

	server->send_packet( this->session_id, move_packet);

	//printf("send_move_packet() my id : %llu  other id : %llu  \n", this->session_id, payload.id);
}

void MMOActor::send_move_packet(MMONpc* other)
{
	sc_packet_move payload;
	payload.header.length = sizeof(sc_packet_move);
	payload.header.type = S2C_MOVE;
	payload.id = other->id;
	payload.x = other->x;
	payload.y = other->y;
	payload.move_time = 0;

	c2::Packet* move_packet = c2::Packet::alloc();

	move_packet->write(&payload, sizeof(sc_packet_move));

	server->send_packet(this->session_id, move_packet);
}

void MMOActor::send_move_packet(MMOActor* other, c2::Packet* move_packet)
{
	move_packet->increase_ref_count();

	server->send_packet(this->session_id, move_packet);
}

void MMOActor::send_leave_packet(MMOActor* other)
{
	sc_packet_leave payload;
	payload.id = other->get_id();
	payload.header.length = sizeof(sc_packet_leave);
	payload.header.type = S2C_LEAVE;


	AcquireSRWLockExclusive(&this->lock);
	this->view_list.erase(other->get_id());
	ReleaseSRWLockExclusive(&this->lock);


	c2::Packet* leave_packet = c2::Packet::alloc();
	leave_packet->write(&payload, sizeof(sc_packet_leave));

	server->send_packet(this->session_id, leave_packet);
}

void MMOActor::send_leave_packet(MMOActor* other, c2::Packet* leave_packet)
{
	AcquireSRWLockExclusive(&this->lock);
	this->view_list.erase(other->get_id());
	ReleaseSRWLockExclusive(&this->lock);

	leave_packet->increase_ref_count();
	server->send_packet(this->session_id, leave_packet);
}

void MMOActor::send_leave_packet(MMONpc* other)
{
	sc_packet_leave payload;
	payload.id = other->id;
	payload.header.length = sizeof(sc_packet_leave);
	payload.header.type = S2C_LEAVE;

	AcquireSRWLockExclusive(&this->lock);
	this->view_list_for_npc.erase(other->id);
	ReleaseSRWLockExclusive(&this->lock);

	c2::Packet* leave_packet = c2::Packet::alloc();
	leave_packet->write(&payload, sizeof(sc_packet_leave));

	server->send_packet(this->session_id, leave_packet);
}

void MMOActor::sned_chat_packet(MMOActor* other)
{

}

#include "contents_enviroment.h"
#include "MMONpcManager.h"

void MMOActor::attack()
{
	int ys[4];
	int xs[4];
	int effective_position_count {};

	int y = this->y; 
	int x = this->x;

	std::unordered_set<int32_t> npc_attack_list;

	// 주변 4방향 좌표를 구하고 검사를 함. 장애물이 있는 곳 or 인덱스 범위를 초과 하는 곳이면 그곳은 제외한다. (  npc만 팬다. ) 
	if ( y - 1 >= 0 && g_zone->has_obstacle(y -1, x) == false )
	{
		ys[effective_position_count] = y - 1; xs[effective_position_count] = x;
		effective_position_count += 1;
	}
	if (y + 1 < c2::constant::MAP_HEIGHT && g_zone->has_obstacle(y + 1, x) == false)
	{
		ys[effective_position_count] = y + 1; xs[effective_position_count] = x;
		effective_position_count += 1;
	}
	if (x - 1 >= 0 && g_zone->has_obstacle(y, x -1) == false)
	{
		ys[effective_position_count] = y; xs[effective_position_count] = x - 1;
		effective_position_count += 1;
	}
	if (x + 1 < c2::constant::MAP_WIDTH && g_zone->has_obstacle(y, x + 1) == false)
	{
		ys[effective_position_count] = y; xs[effective_position_count] = x + 1;
		effective_position_count += 1;
	}
	

	// 
	const MMONear* nears = g_zone->get_near(y, x); // 
	for (int n = 0; n < nears->count; ++n)
	{
		MMOSector* near_sector = nears->sectors[n];			
		std::unordered_set<int32_t>* npcs = &near_sector->npcs;

		AcquireSRWLockShared(&near_sector->lock); // 섹터에 npc에 대한 읽기 작업만.
		for (int32_t npc_id : *npcs)
		{
			MMONpc* npc = g_npc_manager->get_npc(npc_id);   // npc를 구하고...
			
			// if (npc->state != DEATH)  					// npc 상태가 사망이 아니면 
			// 이거를 밑에 실제로 때릴때 해도 됨.

			for (int k = 0; k < effective_position_count; ++k)
			{
				if (ys[k] == npc->y && xs[k] == npc->x) // 좌표가 일치하면 
				{
					npc_attack_list.insert(npc_id);	//공격 범위에 추가.

					break;	// 종료.
				}
			}
		}
		ReleaseSRWLockShared(&near_sector->lock);
	}


	// 락을 풀고 임시 공격 범위 리스트에게 데미지를 입힘
	for (int32_t target_npc_id : npc_attack_list) // 피격될 NPC를 순회하면서 피격 시킴.
	{
		MMONpc* npc = g_npc_manager->get_npc(target_npc_id);   // npc를 구하고...
		//
		//if (npc->state != NPC_DEATH)  					// npc 상태가 사망이 아니면  // 사망상태여도 클라에서 적당히 처리 해줌.
		//	continue;
	
		// 공격 하고 NPC 죽었으면 죽었다고 상태 바꾸기 ㅇㅇ;

		// 누가 죽인건지도 나타내자.	
		
		// auto ret = this->hit(npc);
		 
		// 죽인거면 경험치를 얻고 그에 대한 처리를 한다.

		// NPC도 30초 후 리스폰.
		
		// 전투메시지 나에 대해 경험치를 얻었다고 보냄.

		// npc가 주변에 브로드 캐스팅함. // 죽거나 체력이 깍였다고 .. 
	}


}


