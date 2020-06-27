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

	is_alive = true;

	ReleaseSRWLockExclusive(&lock);
}

void MMOActor::exit()
{
}
//
//void MMOActor::move(char direction)
//{
//	MMONpcManager* mmo_npc_mgr = g_npc_manager;
//
//	// 사본 만들기.
//	int local_y = y;
//	int local_x = x;
//	int local_actor_id = get_id();
//
//
//	// 장애물 체크 등등.
//	switch (direction)
//	{
//	case c2::constant::D_DOWN:
//		if (local_y < MAP_HEIGHT - 1)	local_y++;
//		break;
//	case c2::constant::D_LEFT:
//		if (local_x > 0) local_x--;
//		break;
//	case c2::constant::D_RIGHT:
//		if (local_x < MAP_WIDTH - 1) local_x++;
//		break;
//	case c2::constant::D_UP:
//		if (local_y > 0) local_y--;
//		break;
//	default:
//		size_t* invalid_ptr{}; *invalid_ptr = 02;
//		break;
//	}
//
//
//	x = local_x;
//	y = local_y;
//
//	MMOSector* prev_sector = current_sector;					// view_list 긁어오기.
//	MMOSector* curent_sector = g_zone->get_sector(local_y, local_x);			// view_list 긁어오기.
//
//
//	if (prev_sector != curent_sector)										//섹터가 바뀐 경우.
//	{
//		if (nullptr != prev_sector)
//		{
//			AcquireSRWLockExclusive(&prev_sector->lock);						// 이전 섹터에서 나가기 위해서.
//			prev_sector->actors.erase(local_actor_id);
//			ReleaseSRWLockExclusive(&prev_sector->lock);
//		}
//
//		AcquireSRWLockExclusive(&curent_sector->lock);						// 내 view_list 에 접근하기 쓰기 위해서 락을 얻고 
//		curent_sector->actors.emplace(local_actor_id, this);
//		current_sector = curent_sector;							// 타 스레드에서 접근하면 여기 일로 하고?
//		ReleaseSRWLockExclusive(&curent_sector->lock);						// 내 view_list 에 접근하기 쓰기 위해서 락을 얻고 
//
//		session->request_updating_position(local_y, local_x);
//	}
//
//
//	AcquireSRWLockShared(&lock);
//	std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list;
//	std::unordered_set<int32_t> local_old_view_list_for_npc = view_list_for_npc;
//	ReleaseSRWLockShared(&lock);
//
//
//
//	/// 섹터 구하기...
//	MMOSector* current_sector = curent_sector;
//	const MMONear* nears = current_sector->get_near(local_y, local_x);
//	int				near_cnt = nears->count;
//
//
//	// 내 주변 정보를 긁어 모음.
//	std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
//	std::unordered_set<int32_t>				local_new_view_list_for_npc;
//	for (int n = 0; n < near_cnt; ++n)
//	{
//		AcquireSRWLockShared(&nears->sectors[n]->lock); // sector에 읽기 위해서 락을 얻고 
//		for (auto& actor_iter : nears->sectors[n]->actors)
//		{
//			//if (actor_iter.second->status != ST_ACTIVE) 
//				//continue;
//			if (actor_iter.second == this)
//				continue;
//
//			if (this->is_near(actor_iter.second) == true) // 내 근처가 맞다면 넣음.
//				local_new_view_list.insert(actor_iter);
//		}
//
//		for (auto npc_id : nears->sectors[n]->npcs)
//		{
//			//if (actor_iter.second->status != ST_ACTIVE) 
//				//continue;
//			MMONPC* npc = mmo_npc_mgr->get_npc(npc_id);
//			if (this->is_near(npc) == true) // 내 근처가 맞다면 넣음.
//			{
//				this->wake_up_npc(npc);
//				//local_timer->push_timer_task(npc->id, TTT_ON_WAKE_FOR_NPC, 1, this->session_id);
//
//				local_new_view_list_for_npc.insert(npc_id);
//			}
//		}
//		ReleaseSRWLockShared(&nears->sectors[n]->lock);
//	}
//
//
//
//	c2::Packet* my_move_packet = c2::Packet::alloc();
//	sc_packet_move		sc_move_payload{ {sizeof(sc_packet_move), S2C_MOVE}, local_actor_id, local_x, local_y, 0/*cs_move_payload.move_time*/ };
//
//	my_move_packet->write(&sc_move_payload, sizeof(sc_packet_move));  // 나한테 내 이동전송.
//	g_server->send_packet(this->session_id, my_move_packet);
//
//
//	///////////////////
//	for (auto& new_actor : local_new_view_list)
//	{
//		if (0 == local_old_view_list.count(new_actor.first)) // 이동후 새로 보이는 유저.
//		{
//			this->send_enter_packet(new_actor.second);		// 타인 정보를 나한테 보냄.
//
//			AcquireSRWLockShared(&new_actor.second->lock);
//			if (0 == new_actor.second->view_list.count(this->get_id())) // 타 스레드에서 시야 처리 안 된경우.
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_enter_packet(this);
//			}
//			else // 처리 된경우
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_move_packet(this);   // 상대 시야 리스트에 내가 있는 경우 뷰리스트만 업데이트 한다.
//			}
//		}
//		else  // 기존 뷰리스트에 있던 유저들 
//		{
//			AcquireSRWLockShared(&new_actor.second->lock);					//
//			if (0 != new_actor.second->view_list.count(this->get_id()))	// 
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_move_packet(this);
//			}
//			else	// 이미 나간 경우.
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_enter_packet(this);
//			}
//		}
//	}
//
//	//시야에서 벗어난 플레이어
//	for (auto& old_it : local_old_view_list)
//	{
//		if (0 == local_new_view_list.count(old_it.first))
//		{
//			this->send_leave_packet(old_it.second);
//
//			AcquireSRWLockShared(&old_it.second->lock);
//			if (0 != old_it.second->view_list.count(this->get_id()))
//			{
//				ReleaseSRWLockShared(&old_it.second->lock);
//				old_it.second->send_leave_packet(this);
//			}
//			else	// 이미 다른 스레드에서 지워준 경우.
//			{
//				ReleaseSRWLockShared(&old_it.second->lock);
//			}
//		}
//	}
//
//
//	// 유저가 npc한테 하는 동작 추가.
//		///////////////////
//	for (int32_t npc_id : local_new_view_list_for_npc)
//	{
//		if (0 == local_old_view_list_for_npc.count(npc_id)) // 이동후 새로 보이는 NPC
//		{
//			MMONPC* npc = mmo_npc_mgr->get_npc(npc_id);
//
//			this->send_enter_packet(npc); // 이 npc 정보를 나한테 보냄.
//
//			AcquireSRWLockExclusive(&npc->lock);
//			npc->view_list.emplace(this->get_id(), this);
//			ReleaseSRWLockExclusive(&npc->lock);
//		}
//	}
//
//	// 기존 시야에 있었고 이동후 없어진 녀석은 제거.
//	for (int32_t old_npc_id : local_old_view_list_for_npc)
//	{
//		if (0 == local_new_view_list_for_npc.count(old_npc_id))
//		{
//			//AcquireSRWLockExclusive(&this->lock);
//			//this->view_list_for_npc.erase(old_npc_id);
//			//ReleaseSRWLockExclusive(&this->lock);
//			// 떠나는건 npc가 알아서 해줌. // 얘는 1초에 한번이기도 하고 내가 반영을 안함.
//			MMONPC* npc = mmo_npc_mgr->get_npc(old_npc_id);
//
//			this->send_leave_packet(npc); // 이 npc 정보를 나한테 보냄.
//
//			AcquireSRWLockExclusive(&npc->lock);
//			npc->view_list.erase(this->get_id());
//			ReleaseSRWLockExclusive(&npc->lock);
//		}
//	}
//}

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
	}

	sc_packet_stat_change stat_payload;
	
	stat_payload.hp					= this->hp;
	stat_payload.level				= this->level;
	stat_payload.exp				= this->current_exp;

	ReleaseSRWLockExclusive(&this->lock);

	stat_payload.header.type = S2C_STAT_CHANGE;
	stat_payload.header.length = sizeof(sc_packet_stat_change);

	c2::Packet* exp_packet = c2::Packet::alloc();
	exp_packet->write(&stat_payload, sizeof(sc_packet_stat_change));

	g_server->send_packet(this->session_id, exp_packet);
	// db update 
	
}

void MMOActor::decrease_exp(int32_t exp) // 사망시..
{
	AcquireSRWLockExclusive(&this->lock);
	this->current_exp -= exp;
	if (current_exp <= 0)
	{
		current_exp = 0;
	}

	sc_packet_stat_change stat_payload;
	stat_payload.header.type = S2C_STAT_CHANGE;
	stat_payload.header.length = sizeof(sc_packet_stat_change);
	stat_payload.hp = this->hp;
	stat_payload.level = this->level;
	stat_payload.exp = this->current_exp;

	c2::Packet* exp_packet = c2::Packet::alloc();
	exp_packet->write(&stat_payload, sizeof(sc_packet_stat_change));

	g_server->send_packet(this->session_id, exp_packet);

	ReleaseSRWLockExclusive(&this->lock);
}

void MMOActor::increase_hp(int32_t hp)
{
	AcquireSRWLockExclusive(&this->lock);
	this->hp += hp;
	if (hp > 200)
	{
		current_exp = 200;
	}

	sc_packet_stat_change stat_payload;
	stat_payload.hp = this->hp;
	stat_payload.level = this->level;
	stat_payload.exp = this->current_exp;

	ReleaseSRWLockExclusive(&this->lock); // 여기까지만 락이 필요함.

	stat_payload.header.type = S2C_STAT_CHANGE;
	stat_payload.header.length = sizeof(sc_packet_stat_change);

	c2::Packet* exp_packet = c2::Packet::alloc();
	exp_packet->write(&stat_payload, sizeof(sc_packet_stat_change));

	g_server->send_packet(this->session_id, exp_packet);
}

void MMOActor::decrease_hp(MMONPC* npc, int32_t damage)
{
	AcquireSRWLockExclusive(&this->lock);

	// 공격 하고 NPC 죽었으면 죽었다고 상태 바꾸기 ㅇㅇ;
	if (this->is_alive == false) // 죽은 상태면  고인 건들지 말고 사라진다.
	{
		ReleaseSRWLockExclusive(&this->lock);
	}

	// 체력 처리.
	hp -= damage;
	if (hp <= 0)
	{
		// 사망시 처리. 
		is_alive = false;

		local_timer->push_timer_task(session_id, TTT_RESPAWN_FOR_NPC, 30'000, 0);
	}
	else
	{
		// 아닐시 해야 하는거.

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

	ReleaseSRWLockExclusive(&this->lock);
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

	payload.o_type = 1;

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
}

void MMOActor::wake_up_npc(MMONPC* npc)
{
	if (npc->is_active == NPC_SLEEP) // false
	{
		//if (npc->is_alive == false)
		//{
		//	return;
		//}

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
			MMONPC* npc = g_npc_manager->get_npc(npc_id);   // npc를 구하고...
			
			// if (npc->state != DEATH)  					// npc 상태가 사망이 아니면 
			// 이거를 밑에 실제로 때릴때 해도 됨.
			for (int k = 0; k < effective_position_count; ++k)
			{
				if (ys[k] == npc->y && xs[k] == npc->x) // 좌표가 일치하면 
				{
					npc_attack_list.insert(npc_id);	//공격 대상에 추가.

					break;	// 종료.
				}
			}
		}
		ReleaseSRWLockShared(&near_sector->lock);
	}


	// 락을 풀고 임시 공격 범위 리스트에게 데미지를 입힘
	for (int32_t target_npc_id : npc_attack_list) // 피격될 NPC를 순회하면서 피격 시킴.
	{
		MMONPC* npc = g_npc_manager->get_npc(target_npc_id);   // npc를 구하고...

		if (npc->is_alive == true)
		{
			npc->decrease_hp(this, c2::constant::TEST_DMG);
			if (npc->type < NT_COMBAT_FIXED) // 평화 몹이면
			{
				npc->set_target(this);
				wake_up_npc(npc);
			}
		}
	}
}


