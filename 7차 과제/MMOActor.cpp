#include "pre_compile.h"
#include "MMOSession.h"
#include "MMOZone.h"
#include "MMOActor.h"
#include "util/TimeScheduler.h"
#include "MMONPCManager.h"
#include "MMOServer.h"
#include "MMONpc.h"
#include "MMODBTask.h"
#include <unordered_set>
#include "MMOSystmeMessage.h"
#include <cmath>

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

	is_alive = true;

	ReleaseSRWLockExclusive(&lock);
}

void MMOActor::reset_when_respawn()
{
	current_sector = nullptr;

	hp = 200;

	current_exp /= 2;

	this->view_list.clear();
	this->view_list_for_npc.clear();

	zone = this->zone;
	is_alive = true;
	
	this->y = fast_rand() % 300;
	this->x = fast_rand() % 300;

	for (;;)
	{
		if (g_zone->has_obstacle(this->y, this->x) == true)
		{
			this->y = fast_rand() % 300;
			this->x = fast_rand() % 300;
		}
		else
		{
			break;
		}
	}
}

void MMOActor::exit()
{
}

void MMOActor::respawn()
{
	reset_when_respawn();

	session->enter_zone();

	//session->request_change_status(hp, level, current_exp);

	send_chat_packet(system_msg_respawn);
	send_stat_change();
}

void MMOActor::start_recover_hp()
{
	local_timer->push_timer_task(this->session_id, TTT_USER_RECOVER_HP, 10* 1000, NULL);
}

void MMOActor::reset_data(const LoadActorTask* task)
{
	AcquireSRWLockExclusive(&lock);

	session_id = task->session_id;
	user_id = task->user_id;
	zone = nullptr;

	current_exp = task->exp;
	levelup_exp = task->level * 100;
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
	levelup_exp = 100;
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

bool MMOActor::is_near(MMONPC* other)
{
	if (abs(this->x - other->x) > FOV_HALF_WIDTH) return false;
	if (abs(this->y - other->y) > FOV_HALF_HEIGHT) return false;

	return true;
}

void MMOActor::increase_exp(int32_t exp)
{
	AcquireSRWLockExclusive(&this->lock);
	this->current_exp += exp;
	if (current_exp >= levelup_exp)
	{
		current_exp -= levelup_exp;
		levelup_exp *= 2;
		level += 1;
		send_chat_packet(system_msg_levelup);
	}
	ReleaseSRWLockExclusive(&this->lock);

	sc_packet_stat_change stat_payload;
	
	stat_payload.hp					= this->hp;
	stat_payload.level				= this->level;
	stat_payload.exp				= this->current_exp;
	stat_payload.header.type		= S2C_STAT_CHANGE;
	stat_payload.header.length		= sizeof(sc_packet_stat_change);

	c2::Packet* exp_packet = c2::Packet::alloc();
	exp_packet->write(&stat_payload, sizeof(sc_packet_stat_change));

	g_server->send_packet(this->session_id, exp_packet);
	
	// db update 
	send_chat_packet(system_msg_get_exp);

	//this->send_chat_packet();
	//session->request_change_status(hp, level, current_exp);
}

void MMOActor::decrease_exp(int32_t exp) // 사망시..
{
	AcquireSRWLockExclusive(&this->lock);
	this->current_exp -= exp;
	if (current_exp <= 0)
	{
		current_exp = 0;
	}
	ReleaseSRWLockExclusive(&this->lock);

	sc_packet_stat_change stat_payload;
	stat_payload.header.type = S2C_STAT_CHANGE;
	stat_payload.header.length = sizeof(sc_packet_stat_change);
	stat_payload.hp = this->hp;
	stat_payload.level = this->level;
	stat_payload.exp = this->current_exp;

	c2::Packet* exp_packet = c2::Packet::alloc();
	exp_packet->write(&stat_payload, sizeof(sc_packet_stat_change));

	g_server->send_packet(this->session_id, exp_packet);

	//session->request_change_status(hp, level, current_exp);
}

void MMOActor::increase_hp(int32_t hp)
{
	
	AcquireSRWLockExclusive(&this->lock);
	int prev_hp = hp;
	this->hp += hp;
	if (this->hp > 200)
	{
		this->hp = 200;
	}
	ReleaseSRWLockExclusive(&this->lock); // 여기까지만 락이 필요함.

	if (prev_hp != this->hp) // 체력이 변경 디었을때만 보냄.
	{
		sc_packet_stat_change stat_payload;
		stat_payload.hp = this->hp;
		stat_payload.level = this->level;
		stat_payload.exp = this->current_exp;
		stat_payload.header.type = S2C_STAT_CHANGE;
		stat_payload.header.length = sizeof(sc_packet_stat_change);

		c2::Packet* exp_packet = c2::Packet::alloc();
		exp_packet->write(&stat_payload, sizeof(sc_packet_stat_change));

		g_server->send_packet(this->session_id, exp_packet);
		//session->request_change_status(hp, level, current_exp);
	}
}

void MMOActor::decrease_hp(MMONPC* npc, int32_t damage)
{
	AcquireSRWLockExclusive(&this->lock);

	// 체력 처리.
	hp -= damage;
	if (hp <= 0)
	{
		// 사망시 처리. 
		if (this->is_alive != false && true == InterlockedExchange8((volatile CHAR*)&is_alive, 0))// = false; // 공격 하고 NPC 죽었으면 죽었다고 상태 바꾸기 ㅇㅇ
		{
			AcquireSRWLockExclusive(&current_sector->lock); // 현재 섹터 나가기. // 다른 클라접근 하기 힘들게..
			current_sector->actors.erase(this->get_id());
			ReleaseSRWLockExclusive(&current_sector->lock);

			send_stat_change();
			send_chat_packet(system_msg_die);

			for (auto& iter : view_list)			// 뷰리스트 정리함 내가 없어졌다고. 
			{
				MMOActor* neighbor = iter.second;

				neighbor->send_leave_packet(this); // 내 시야리스트 플레이에게 내가 나간다고 알림.
				this->send_leave_packet_without_updating_viewlist(neighbor); // 상대 정보 내 클라이언트에서 내보냄.
			}


			for (int32_t npc_id : view_list_for_npc)			// 뷰리스트 정리함 내가 없어졌다고. 
			{
				MMONPC* neighbor_npc = g_npc_manager->get_npc(npc_id);

				AcquireSRWLockExclusive(&neighbor_npc->lock);
				neighbor_npc->view_list.erase(this->get_id()); // 내 시야리스트 플레이에게 내가 나간다고 알림.
				ReleaseSRWLockExclusive(&neighbor_npc->lock);

				this->send_leave_packet_without_updating_viewlist(neighbor_npc); // 상대 정보 내 클라이언트에서 내보냄.
			}

			ReleaseSRWLockExclusive(&this->lock); // 여기선 락을 푼다.

			// 나한테도 사망 메시지 보내야함.
			local_timer->push_timer_task(session_id, TTT_RESPAWN_FOR_PLAYER, 30'000, 0);
		}
		else
		{
			ReleaseSRWLockExclusive(&this->lock);
			return;
		}
	}
	else // 아닐시 해야 하는거.
	{
		ReleaseSRWLockExclusive(&this->lock);

		send_stat_change();
	}

	//session->request_change_status(hp, level, current_exp);
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
void MMOActor::send_enter_packet(MMONPC* other)
{
	sc_packet_enter payload;
	payload.id = (int)other->id;
	payload.header.length = sizeof(sc_packet_enter);
	payload.header.type = S2C_ENTER;
	payload.x = other->x;
	payload.y = other->y;
	wcscpy_s(payload.name, other->name);

	payload.o_type = other->type + 1;// / 2) + 1;

	AcquireSRWLockExclusive(&this->lock);
	this->view_list_for_npc.emplace((int)other->id);
	ReleaseSRWLockExclusive(&this->lock);

	c2::Packet* enter_packet = c2::Packet::alloc();
	enter_packet->write(&payload, sizeof(sc_packet_enter));

	g_server->send_packet(this->session_id, enter_packet);
}
// 내 정보만 보냄.. 
void MMOActor::send_enter_packet_without_updating_viewlist(MMONPC* other)
{
	sc_packet_enter payload;
	payload.id = (int)other->id;
	payload.header.length = sizeof(sc_packet_enter);
	payload.header.type = S2C_ENTER;
	payload.x = other->x;
	payload.y = other->y;
	wcscpy_s(payload.name, other->name);
	payload.o_type = other->type + 1;

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
}

void MMOActor::send_stat_change()
{
	// 체력 감소 나한테만 통지.
	sc_packet_stat_change stat_payload;
	stat_payload.header.type = S2C_STAT_CHANGE;
	stat_payload.header.length = sizeof(sc_packet_stat_change);
	stat_payload.hp = this->hp;
	stat_payload.level = this->level;
	stat_payload.exp = this->current_exp;

	c2::Packet* exp_packet = c2::Packet::alloc();
	exp_packet->write(&stat_payload, sizeof(sc_packet_stat_change));

	g_server->send_packet(this->session_id, exp_packet);
}

void MMOActor::wake_up_npc(MMONPC* npc)
{
	if (npc->is_active == NPC_SLEEP) // false
	{
		if (NPC_SLEEP == InterlockedExchange(&npc->is_active, NPC_WORKING) )		// 이전 상태가 자고 있었다면 꺠움.
		{
			// 내가 꺠운 상태.	// 내가 책임지고 일을 시켜야 함.
			local_timer->push_timer_task(npc->id, TTT_ON_WAKE_FOR_NPC, 1000, session_id);
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

void MMOActor::send_move_packet(MMONPC* other)
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

void MMOActor::send_leave_packet(MMONPC* other)
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

void MMOActor::send_leave_packet_without_updating_viewlist(MMOActor* other)
{
	sc_packet_leave payload;
	payload.id = other->get_id();
	payload.header.length = sizeof(sc_packet_leave);
	payload.header.type = S2C_LEAVE;

	c2::Packet* leave_packet = c2::Packet::alloc();
	leave_packet->write(&payload, sizeof(sc_packet_leave));

	server->send_packet(this->session_id, leave_packet);
}

void MMOActor::send_leave_packet_without_updating_viewlist(MMONPC* other)
{
	sc_packet_leave payload;
	payload.id = other->id;
	payload.header.length = sizeof(sc_packet_leave);
	payload.header.type = S2C_LEAVE;


	c2::Packet* leave_packet = c2::Packet::alloc();
	leave_packet->write(&payload, sizeof(sc_packet_leave));

	server->send_packet(this->session_id, leave_packet);
}

void MMOActor::send_chat_packet(const wchar_t* msg)
{
	sc_packet_chat chat_payload;
	chat_payload.header.length = sizeof(sc_packet_chat);
	chat_payload.header.type = S2C_CHAT;
	chat_payload.id = this->session_id;
	wcscpy_s(chat_payload.chat ,msg);

	c2::Packet* chat_packet = c2::Packet::alloc();
	chat_packet->write(&chat_payload, sizeof(sc_packet_chat));

	g_zone->server->send_packet(session_id, chat_packet);
}

#include "contents_enviroment.h"
#include "MMONpcManager.h"

void MMOActor::attack()
{
	int ys[5];
	int xs[5];
	int effective_position_count {};

	int y = this->y; 
	int x = this->x;

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

	ys[effective_position_count] = y;
	xs[effective_position_count] = x;
	effective_position_count += 1;

	AcquireSRWLockShared(&lock); // 섹터에 npc에 대한 읽기 작업만.
	auto& local_view_list_for_npc = view_list_for_npc;
	ReleaseSRWLockShared(&lock);


	for (int32_t npc_id : local_view_list_for_npc)
	{
		MMONPC* npc = g_npc_manager->get_npc(npc_id);   // npc를 구하고...

		for (int k = 0; k < effective_position_count; ++k)
		{
			if (ys[k] == npc->y && xs[k] == npc->x) // 좌표가 일치하면 
			{
				if (npc->is_alive == true)
				{
					npc->decrease_hp(this, c2::constant::TEST_DMG);
					if (npc->type < NT_COMBAT_FIXED) // 평화 몹이면
					{
						npc->set_target(this);
						wake_up_npc(npc);
						break;
					}
				}
			}
		}
	}
}


