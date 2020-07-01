
#include "pre_compile.h"
#include "MMOServer.h"
#include "mmo_function.hpp"
#include "MMONPC.h"
#include "MMOActor.h"
#include "MMONpcManager.h"
#include "MMOZone.h"
#include "MMONear.h"
#include "PathFinder.h"
#include "MMOSystmeMessage.h"


#include "script_api.h"

using namespace c2::constant;


void MMONPC::initialize_vm_and_load_data()
{
	lua_State* lua_vm = luaL_newstate();
	luaL_openlibs(lua_vm);

	int error = luaL_loadfile(lua_vm, "scripts\\npc.lua");
	if (error != 0)
	{
		printf("1lua error %s \n", lua_tostring(lua_vm, -1));
	}
	error = lua_pcall(lua_vm, 0, 0, 0);
	if (error != 0)
	{
		printf("2lua error %s \n", lua_tostring(lua_vm, -1));
	}

	// 
	lua_getglobal(lua_vm, "prepare_npc_script");
	lua_pushnumber(lua_vm, (int32_t)id);
	lua_pcall(lua_vm, 1, 0, 0);

	lua_getglobal(lua_vm, "load_mob_data");
	lua_pushnumber(lua_vm, this->type);		// �Լ��� ���ڷ� �ְ� 
	error = lua_pcall(lua_vm, 1, 6, 0);		// 1 �Ķ����, 6����;
	if (error != 0)
	{
		printf("load_mob_data() lua error %s \n", lua_tostring(lua_vm, -1));
	}

	// return ��
	this->x = this->initial_x =	lua_tonumber(lua_vm, -6);
	this->y = this->initial_y = lua_tonumber(lua_vm, -5);
	this->hp =		lua_tonumber(lua_vm, -4);
	this->dmg =		lua_tonumber(lua_vm, -3);
	this->level =	lua_tonumber(lua_vm, -2);
	this->exp =		lua_tonumber(lua_vm, -1);


	lua_pop(lua_vm, 6); // �� �����.
	// ����Ȳ������ ���̻� lua�� �Ⱦ�.
	//this->lua_vm = lua_vm;
	lua_close(lua_vm);
}

//void MMONPC::move()
//{
//	bool is_isolated = true;
//	int local_y = y;
//	int local_x = x;
//	int local_actor_id = id;
//
//	// ��ֹ� üũ ���.
//	switch (fast_rand() % 4)
//	{
//	case D_DOWN:
//		if (local_y < MAP_HEIGHT - 1)	local_y++;
//		break;
//	case D_LEFT:
//		if (local_x > 0) local_x--;
//		break;
//	case D_RIGHT:
//		if (local_x < MAP_WIDTH - 1) local_x++;
//		break;
//	case D_UP:
//		if (local_y > 0) local_y--;
//		break;
//	default:
//		size_t* invalid_ptr{}; *invalid_ptr = 0;
//		break;
//	}
//
//	x = local_x;
//	y = local_y;
//
//
//
//	MMOSector* new_sector = zone->get_sector(local_y, local_x);			// view_list �ܾ����.
//	// ���Ͱ� ����Ǹ� �ٲٱ�.
//	if (current_sector != new_sector)
//	{
//		AcquireSRWLockExclusive(&current_sector->lock); // ���� ���� ������
//		current_sector->npcs.erase(id);
//		ReleaseSRWLockExclusive(&current_sector->lock);
//
//		// ���� ���� �鰡��.
//		AcquireSRWLockExclusive(&new_sector->lock);
//		this->current_sector = new_sector;
//		new_sector->npcs.insert(id);
//		ReleaseSRWLockExclusive(&new_sector->lock);
//	}
//
//
//	// �丮��Ʈ ����.
//	AcquireSRWLockExclusive(&lock); // // �� �ɰ�...
//	std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list;
//	ReleaseSRWLockExclusive(&lock); // // �� �ɰ�...
//
//
//	// �ֺ��� �Ѹ���.
//	MMOSector*		current_sector	= new_sector;
//	const MMONear*	nears			= current_sector->get_near(local_y, local_x);
//	int				near_cnt		= nears->count;
//
//	
//	// �� �ֺ� ������ �ܾ� ����.
//	std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
//	for (int n = 0; n < near_cnt; ++n)
//	{
//		AcquireSRWLockShared(&nears->sectors[n]->lock); // sector�� �б� ���ؼ� ���� ��� 
//		for (auto& actor_iter : nears->sectors[n]->actors)
//		{
//			//if (actor_iter.second->status != ST_ACTIVE) 		//continue;
//			if (actor_iter.second->is_near(this) == true) // �� ��ó�� �´ٸ� ����.
//			{
//				is_isolated = false;
//				local_new_view_list.insert(actor_iter);
//			}
//		}
//		ReleaseSRWLockShared(&nears->sectors[n]->lock);
//	}
//
//	for (auto& new_actor : local_new_view_list)
//	{
//		if (0 == local_old_view_list.count(new_actor.first))	// �̵��� ���� ���̴� ����.
//		{
//			this->update_entering_actor(new_actor.second);		// ���� ���̴� ���� ������ NPC���� ������Ʈ ��.
//
//			AcquireSRWLockShared(&new_actor.second->lock);
//			if (0 == new_actor.second->view_list_for_npc.count(this->id)) // Ÿ �����忡�� �þ� ó�� �� �Ȱ��.
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_enter_packet(this);
//			}
//			else // ó�� �Ȱ��
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_move_packet(this);   // ��� �þ� ����Ʈ�� ���� �ִ� ��� �丮��Ʈ�� ������Ʈ �Ѵ�.
//			}
//		}
//		else  // ���� �丮��Ʈ�� �ִ� ������ 
//		{
//			AcquireSRWLockShared(&new_actor.second->lock);					//
//			if (0 != new_actor.second->view_list_for_npc.count(this->id))	// 
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_move_packet(this);
//			}
//			else	// �̹� ���� ���.
//			{
//				ReleaseSRWLockShared(&new_actor.second->lock);
//
//				new_actor.second->send_enter_packet(this);
//			}
//		}
//	}
//
//
//	//�þ߿��� ��� �÷��̾�
//	for (auto& old_it : local_old_view_list)
//	{
//		if (0 == local_new_view_list.count(old_it.first))		// ���� �� �þ߿� ���� �÷��̾�.
//		{														// 
//			this->update_leaving_actor(old_it.second);			// npc �þ߿��� ������ �������� ���� ����.
//			
//			AcquireSRWLockShared(&old_it.second->lock);			// 
//			if (0 != old_it.second->view_list_for_npc.count(this->id)) // Ŭ�� �þ߿� ���� �ִ� ���
//			{
//				ReleaseSRWLockShared(&old_it.second->lock);
//				old_it.second->send_leave_packet(this);
//			}
//			else	// �̹� �ٸ� �����忡�� ������ ���.
//			{
//				ReleaseSRWLockShared(&old_it.second->lock);
//			}
//		}
//	}
//
//	// �ֺ��� �ƹ��� ���ٸ�?
//	if (is_isolated == true)
//	{
//		// �� ����� �ǵ鿴���� �������� ó������.
//		// Ÿ �����忡�� �����ؼ� ���� ���� ç �� �ֵ���...
//		is_active = NPC_SLEEP;
//	}
//	else // �ִٸ� ������Ʈ ���.
//	{
//		// is_active ������ �� ������ �־ ������Ʈ ������ ���� ������ ���� ����.
//		local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
//	}
//}

void MMONPC::move_to_anywhere()
{
	if (false == this->is_alive)
	{
		return; 
	}

	bool is_isolated = true;
	int local_y = y;
	int local_x = x;
	int local_actor_id = id;

	switch (fast_rand() % 4)
	{
	case D_DOWN:
		if (local_y < MAP_HEIGHT - 1)	local_y++;
		break;
	case D_LEFT:
		if (local_x > 0) local_x--;
		break;
	case D_RIGHT:
		if (local_x < MAP_WIDTH - 1) local_x++;
		break;
	case D_UP:
		if (local_y > 0) local_y--;
		break;
	default:
		size_t* invalid_ptr{}; *invalid_ptr = 0;
		break;
	}

	x = local_x;
	y = local_y;

	MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list �ܾ����.
	// ���Ͱ� ����Ǹ� �ٲٱ�.
	if (current_sector != new_sector)
	{
		AcquireSRWLockExclusive(&current_sector->lock); // ���� ���� ������
		current_sector->npcs.erase(id);
		ReleaseSRWLockExclusive(&current_sector->lock);

		// ���� ���� �鰡��.
		AcquireSRWLockExclusive(&new_sector->lock);
		this->current_sector = new_sector;
		new_sector->npcs.insert(id);
		ReleaseSRWLockExclusive(&new_sector->lock);
	}


	// �丮��Ʈ ����.
	AcquireSRWLockShared(&lock); // read �� �ɰ�...
	std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
	ReleaseSRWLockShared(&lock);


	// ���� ���ķ� ����ϸ�... 

	// �ֺ��� �Ѹ���.
	MMOSector*		current_sector	= new_sector;
	const MMONear*	nears			= current_sector->get_near(local_y, local_x);
	int				near_cnt		= nears->count;


	// �� �ֺ� ������ �ܾ� ����.
	std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
	for (int n = 0; n < near_cnt; ++n)
	{
		AcquireSRWLockShared(&nears->sectors[n]->lock); // sector�� �б� ���ؼ� ���� ��� 
		for (auto& actor_iter : nears->sectors[n]->actors)
		{
			//if (actor_iter.second->status != ST_ACTIVE) 		//continue;
			if (actor_iter.second->is_near(this) == true) // �� ��ó�� �´ٸ� ����.
			{
				is_isolated = false;
				local_new_view_list.insert(actor_iter);
			}
		}
		ReleaseSRWLockShared(&nears->sectors[n]->lock);
	}

	for (auto& new_actor : local_new_view_list)
	{
		if (0 == local_old_view_list.count(new_actor.first))	// �̵��� ���� ���̴� ����.
		{
			this->update_entering_actor(new_actor.second);		// ���� ���̴� ���� ������ NPC���� ������Ʈ ��.

			AcquireSRWLockShared(&new_actor.second->lock);
			if (0 == new_actor.second->view_list_for_npc.count(this->id)) // Ÿ �����忡�� �þ� ó�� �� �Ȱ��.
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_enter_packet(this);
			}
			else // ó�� �Ȱ��
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_move_packet(this);   // ��� �þ� ����Ʈ�� ���� �ִ� ��� �丮��Ʈ�� ������Ʈ �Ѵ�.
			}
		}
		else  // ���� �丮��Ʈ�� �ִ� ������ 
		{
			AcquireSRWLockShared(&new_actor.second->lock);					//
			if (0 != new_actor.second->view_list_for_npc.count(this->id))	// 
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_move_packet(this);
			}
			else	// �̹� ���� ���.
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_enter_packet(this);
			}
		}
	}


	//�þ߿��� ��� �÷��̾�
	for (auto& old_it : local_old_view_list)
	{
		if (0 == local_new_view_list.count(old_it.first))		// ���� �� �þ߿� ���� �÷��̾�.
		{														// 
			this->update_leaving_actor(old_it.second);			// npc �þ߿��� ������ �������� ���� ����.

			AcquireSRWLockShared(&old_it.second->lock);			// 
			if (0 != old_it.second->view_list_for_npc.count(this->id)) // Ŭ�� �þ߿� ���� �ִ� ���
			{
				ReleaseSRWLockShared(&old_it.second->lock);
				old_it.second->send_leave_packet(this);
			}
			else	// �̹� �ٸ� �����忡�� ������ ���.
			{
				ReleaseSRWLockShared(&old_it.second->lock);
			}
		}
	}

	if (is_isolated == true)
	{
		is_active = NPC_SLEEP;
		// ���� Ȯ�ο� �۾� // ���۾��߿� ���� 
		//local_timer->push_timer_task(this->id, TTT_ON_SLEEP, 1000, 0);
	}
	else // �ִٸ� ������Ʈ ���.
	{
		local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
	}
}

void MMONPC::initialize(size_t id_base)
{
	current_sector = nullptr;
	
	id = id_base + c2::constant::NPC_ID_OFFSET;
	max_hp = 200;

	is_alive = true;

	const wchar_t* npc_tag = L"NPC";
	memcpy(this->name, npc_tag, 8);

	target = nullptr;
	//hp = 200;
	//x = rand() % c2::constant::MAP_WIDTH;
	//y = rand() % c2::constant::MAP_HEIGHT;
	zone = g_zone;
	is_active = NPC_SLEEP;
}

void MMONPC::reset()
{
	current_sector = nullptr;
	target = nullptr;
	has_target = false;


	hp = 200;
	max_hp = 200;

	target = nullptr;

	this->view_list.clear();

	//x = rand() % c2::constant::MAP_WIDTH;
	//y = rand() % c2::constant::MAP_HEIGHT;

	zone = this->zone;
	is_alive = true;
	is_active = NPC_SLEEP;
}

void MMONPC::respawn() // �ٵ� �̶��� �ƹ��� �𸣴� �����̱� ������ ���� �Ȱɰ� �ص� �Ǳ��ϴµ�...
{
	zone->enter_npc(this);

	// �ֺ� �þ� ���.
	MMOSector*		current_sector = zone->get_sector(y , x);
	const MMONear*	nears		= g_zone->get_near(this->y, this->x);
	int				near_cnt	= nears->count;

	std::unordered_map<int32_t, MMOActor*>	local_view_list;
	for (size_t n{ }; n < nears->count; ++n)
	{
		MMOSector* near_sector = nears->sectors[n];
		AcquireSRWLockShared(&near_sector->lock); //sector�� �б� ���ؼ� ���� ��� 
		
		for (auto& other_iter : near_sector->actors)
		{
			if (other_iter.second->is_near(this) == true) // ��ó�� �´ٸ� ����.
				local_view_list.insert(other_iter);
		}

		//NPC ó�� ����.
		ReleaseSRWLockShared(&near_sector->lock);
	}


	// �����鿡�� ������ �˸��� �� �丮��Ʈ�� �߰���.
	for (auto& iter : local_view_list)
	{
		MMOActor* other = iter.second;
		
		other->send_enter_packet(this); // NPC ������ �������� ������ �丮��Ʈ�߰�..
		// �� �丮��Ʈ �߰��� �ڷ� �̷�.
	}

	// �� �丮��Ʈ ����
	AcquireSRWLockExclusive(&this->lock);							//�� view_list �� �����ϱ� �б� ���ؼ� ���� ��� 
	this->view_list = std::move(local_view_list);
	ReleaseSRWLockExclusive(&this->lock);
}

void MMONPC::attack_for_peace()
{
	bool local_has_target = has_target;

	if (false == local_has_target || false == is_in_attack_range(target)) // ����� ���ų�... �������� ������ٸ� ����.
	{
		//is_active = NPC_SLEEP;
		// ���� 1ȸ �� ������ �ʸ�.
		return;
	}

	int ys[5];
	int xs[5];

	int effective_position_count{};

	int y = this->y;
	int x = this->x;

	//std::unordered_map<int32_t, MMOActor*> user_attack_list;

	// �ֺ� 4���� ��ǥ�� ���ϰ� �˻縦 ��. ��ֹ��� �ִ� �� or �ε��� ������ �ʰ� �ϴ� ���̸� �װ��� �����Ѵ�. (  user�� �Ҵ�. ) 
	if (y - 1 >= 0 && g_zone->has_obstacle(y - 1, x) == false)
	{
		ys[effective_position_count] = y - 1; xs[effective_position_count] = x;
		effective_position_count += 1;
	}
	if (y + 1 < c2::constant::MAP_HEIGHT && g_zone->has_obstacle(y + 1, x) == false)
	{
		ys[effective_position_count] = y + 1; xs[effective_position_count] = x;
		effective_position_count += 1;
	}
	if (x - 1 >= 0 && g_zone->has_obstacle(y, x - 1) == false)
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

	AcquireSRWLockShared(&lock); // ���Ϳ� npc�� ���� �б� �۾���.
	auto& local_view_list = view_list;
	ReleaseSRWLockShared(&lock);

	for (auto& user_it : local_view_list)
	{
		MMOActor* user = user_it.second;   // 

		for (int k = 0; k < effective_position_count; ++k)
		{
			if (ys[k] == user->y && xs[k] == user->x)	// ��ǥ�� ��ġ�ϸ� 
			{
				if (user->is_alive == true)
				{
					user->decrease_hp(this, this->dmg);
					send_chat_packet_to_target(user, system_msg_log_attack);
				}
			}
		}
	}
}

void MMONPC::die()
{
}
	
void MMONPC::send_chatting_to_actor(int32_t actor_id ,wchar_t* message)
{
	sc_packet_chat chat_payload;
	chat_payload.header.length = sizeof(sc_packet_chat);
	chat_payload.header.type = S2C_CHAT;
	chat_payload.id = this->id;
	wcscpy_s(chat_payload.chat, message);
	
	c2::Packet* chat_packet = c2::Packet::alloc();
	chat_packet->write(&chat_payload, sizeof(sc_packet_chat));

	g_zone->server->send_packet(actor_id, chat_packet);
}

void MMONPC::update_for_fixed_peace()
{
	if (false == this->is_alive) // ���� ���� ���¸� ����.
	{
		is_active = NPC_SLEEP;
		has_target = false;

		return;
	}

	MMOActor* local_target		= this->target; 
	bool	  local_has_target	= this->has_target;
	if (false == has_target || false == is_in_attack_range(local_target)) // ����� ���ų� �Ÿ��� �־����� ����.
	{
		is_active = NPC_SLEEP;
		has_target = false;

		return;
	}

	if (nullptr != local_target && true == local_target->is_alive)			// Ÿ���� �����ÿ��� �� ã�ƿ�.
	{
		bool is_isolated = true;
		int local_actor_id = id;

		int temp_local_y;
		int temp_local_x;
		int local_y = temp_local_y = y;
		int local_x = temp_local_x = x;

		int temp_local_target_y;
		int temp_local_target_x;
		int local_target_y = temp_local_target_y = target->y;
		int local_target_x = temp_local_target_x = target->x;



		bool has_obstacle = true;
		for (;;) // ���� ��ο� ��ֹ��� ���ٸ� �׳�   		// �ִ� ��� ã��.
		{
			if (temp_local_x > temp_local_target_x) // x ��ǥ
			{
				temp_local_x -= 1;
			}
			else if (temp_local_x < temp_local_target_x)
			{
				temp_local_x += 1;
			}

			if (temp_local_y > temp_local_target_y) // y ��ǥ
			{
				temp_local_y -= 1;
			}
			else if (temp_local_y < temp_local_target_y)
			{
				temp_local_y += 1;
			}

			if (c2::global::obstacle_table[(temp_local_y * c2::constant::MAP_HEIGHT) + temp_local_x]) // �߰��� ��ֹ��� �ִٸ�?
			{
				break; // ���� -> astr�� ��ã��.
			}

			if (temp_local_x == temp_local_target_x && temp_local_y == temp_local_target_y) // ���� ���� ��޹��� ��������?
			{
				if (local_x > local_target_x) // x ��ǥ
				{
					local_x -= 1;
				}
				else if (local_x < local_target_x)
				{
					local_x += 1;
				}

				if (local_y > local_target_y) // y ��ǥ
				{
					local_y -= 1;
				}
				else if (local_y < local_target_y)
				{
					local_y += 1;
				}

				has_obstacle = false;

				if (local_x != local_target_x || local_y != local_target_y)
				{
					x = local_x;
					y = local_y;

					break;
				}
				else
				{
					local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
					return;
				}
			}
		}

		if (true == has_obstacle)  // ��ֹ��� �ִٸ� astar�� �� ã��.
		{
			printf("astar");

			if (true == PathFindingHelper->NewPath(local_x, local_y, local_target_x, local_target_y))
			{
				if (true == PathFindingHelper->PathNextNode())
				{
					x = local_x = PathFindingHelper->NodeGetX();
					y = local_y = PathFindingHelper->NodeGetY();
				}
			}
		}

	


		MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list �ܾ����.
		// ���Ͱ� ����Ǹ� �ٲٱ�.
		if (current_sector != new_sector)
		{
			AcquireSRWLockExclusive(&current_sector->lock); // ���� ���� ������
			current_sector->npcs.erase(id);
			ReleaseSRWLockExclusive(&current_sector->lock);

			// ���� ���� �鰡��.
			AcquireSRWLockExclusive(&new_sector->lock);
			this->current_sector = new_sector;
			new_sector->npcs.insert(id);
			ReleaseSRWLockExclusive(&new_sector->lock);
		}


		// �丮��Ʈ ����.
		AcquireSRWLockShared(&lock); // read �� �ɰ�...
		std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
		ReleaseSRWLockShared(&lock);


		// �ֺ��� �Ѹ���.
		MMOSector*		current_sector	= new_sector;
		const MMONear*	nears			= current_sector->get_near(local_y, local_x);
		int				near_cnt		= nears->count;

		// �� �ֺ� ������ �ܾ� ����.
		std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
		for (int n = 0; n < near_cnt; ++n)
		{
			AcquireSRWLockShared(&nears->sectors[n]->lock); // sector�� �б� ���ؼ� ���� ��� 
			for (auto& actor_iter : nears->sectors[n]->actors)
			{
				//if (actor_iter.second->status != ST_ACTIVE) 		//continue;
				if (actor_iter.second->is_near(this) == true) // �� ��ó�� �´ٸ� ����.
				{
					is_isolated = false;
					local_new_view_list.insert(actor_iter);
				}
			}
			ReleaseSRWLockShared(&nears->sectors[n]->lock);
		}

		for (auto& new_actor : local_new_view_list)
		{
			if (0 == local_old_view_list.count(new_actor.first))	// �̵��� ���� ���̴� ����.
			{
				this->update_entering_actor(new_actor.second);		// ���� ���̴� ���� ������ NPC���� ������Ʈ ��.

				AcquireSRWLockShared(&new_actor.second->lock);
				if (0 == new_actor.second->view_list_for_npc.count(this->id)) // Ÿ �����忡�� �þ� ó�� �� �Ȱ��.
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_enter_packet(this);
				}
				else // ó�� �Ȱ��
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_move_packet(this);   // ��� �þ� ����Ʈ�� ���� �ִ� ��� �丮��Ʈ�� ������Ʈ �Ѵ�.
				}
			}
			else  // ���� �丮��Ʈ�� �ִ� ������ 
			{
				AcquireSRWLockShared(&new_actor.second->lock);					//
				if (0 != new_actor.second->view_list_for_npc.count(this->id))	// 
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_move_packet(this);
				}
				else	// �̹� ���� ���.
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_enter_packet(this);
				}
			}

			if (true == is_in_attack_range(new_actor.second))
			{
				set_target(new_actor.second);
			}
		}


		//�þ߿��� ��� �÷��̾�
		for (auto& old_it : local_old_view_list)
		{
			if (0 == local_new_view_list.count(old_it.first))		// ���� �� �þ߿� ���� �÷��̾�.
			{														// 
				this->update_leaving_actor(old_it.second);			// npc �þ߿��� ������ �������� ���� ����.

				AcquireSRWLockShared(&old_it.second->lock);			// 
				if (0 != old_it.second->view_list_for_npc.count(this->id)) // Ŭ�� �þ߿� ���� �ִ� ���
				{
					ReleaseSRWLockShared(&old_it.second->lock);
					old_it.second->send_leave_packet(this);
				}
				else	// �̹� �ٸ� �����忡�� ������ ���.
				{
					ReleaseSRWLockShared(&old_it.second->lock);
				}
			}
		}

		if (is_isolated == true)
		{
			is_active = NPC_SLEEP;
			//target = nullptr;
			has_target = false;
			// ���� Ȯ�ο� �۾� // ���۾��߿� ���� 
		}
		else // �ִٸ� ������Ʈ ���.
		{
			local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
		}
	}
	else
	{
		is_active = NPC_SLEEP;
		//target = nullptr;
		has_target = false;
	}
}

void MMONPC::update_for_peace()
{
	if (false == this->is_alive) // ���� ���� ���¸� ����.
	{
		is_active = NPC_SLEEP;
		has_target = false;

		return;
	}

	MMOActor* local_target = this->target;
	bool	  local_has_target = this->has_target;

	if ( false == local_has_target || false == is_in_attack_range(target) ) // ����� ���ų� �Ÿ��� �־����� ����.
	{
		is_active = NPC_SLEEP;

		return;
	}


	if (nullptr != local_target && true == local_target->is_alive)			// Ÿ���� �����ÿ��� �� ã�ƿ�.
	{
		bool is_isolated = true;
		int local_actor_id = id;

		int temp_local_y;
		int temp_local_x;
		int local_y = temp_local_y = y;
		int local_x = temp_local_x = x;
		
		int temp_local_target_y;
		int temp_local_target_x;
		int local_target_y = temp_local_target_y = target->y;
		int local_target_x = temp_local_target_x = target->x;

		

		bool has_obstacle = true;
		for (;;) // ���� ��ο� ��ֹ��� ���ٸ� �׳�   		// �ִ� ��� ã��.
		{
			if (temp_local_x > temp_local_target_x) // x ��ǥ
			{
				temp_local_x -= 1;
			}
			else if (temp_local_x < temp_local_target_x)
			{
				temp_local_x += 1;
			}

			if (temp_local_y > temp_local_target_y) // y ��ǥ
			{
				temp_local_y -= 1;
			}
			else if (temp_local_y < temp_local_target_y)
			{
				temp_local_y += 1;
			}

			if (c2::global::obstacle_table[(temp_local_y * c2::constant::MAP_HEIGHT) + temp_local_x]) // �߰��� ��ֹ��� �ִٸ�?
			//if (c2::global::obstacle_table[(temp_local_x * c2::constant::MAP_WIDTH) + temp_local_y]) // �߰��� ��ֹ��� �ִٸ�?
			{
				break; // ���� -> astr�� ��ã��.
			}

			if (temp_local_x == temp_local_target_x && temp_local_y == temp_local_target_y ) // ���� ���� ��޹��� ��������?
			{
				if (local_x > local_target_x) // x ��ǥ
				{
					local_x -= 1;
				}
				else if (local_x < local_target_x)
				{
					local_x += 1;
				}

				if (local_y > local_target_y) // y ��ǥ
				{
					local_y -= 1;
				}
				else if (local_y < local_target_y)
				{
					local_y += 1;
				}

				has_obstacle = false;

				if (local_x != local_target_x || local_y != local_target_y)
				{
					x = local_x;
					y = local_y;

					break;
				}
				else
				{
					local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
					return;
				}
			}
		}



		if(true == has_obstacle)  // ��ֹ��� �ִٸ� astar�� �� ã��.
		{
			printf("astars\n");

			PathFindingHelper->NewPath(local_x, local_y, local_target->x, local_target->y);

			x = local_x = PathFindingHelper->NodeGetX();
			y = local_y = PathFindingHelper->NodeGetY();
		}



		MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list �ܾ����.
		// ���Ͱ� ����Ǹ� �ٲٱ�.
		if (current_sector != new_sector)
		{
			AcquireSRWLockExclusive(&current_sector->lock); // ���� ���� ������
			current_sector->npcs.erase(id);
			ReleaseSRWLockExclusive(&current_sector->lock);

			// ���� ���� �鰡��.
			AcquireSRWLockExclusive(&new_sector->lock);
			this->current_sector = new_sector;
			new_sector->npcs.insert(id);
			ReleaseSRWLockExclusive(&new_sector->lock);
		}


		// �丮��Ʈ ����.
		AcquireSRWLockShared(&lock); // read �� �ɰ�...
		std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
		ReleaseSRWLockShared(&lock);



		// �ֺ��� �Ѹ���.
		MMOSector*		current_sector	= new_sector;
		const MMONear*	nears			= current_sector->get_near(local_y, local_x);
		int				near_cnt		= nears->count;



		// �� �ֺ� ������ �ܾ� ����.
		std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
		for (int n = 0; n < near_cnt; ++n)
		{
			AcquireSRWLockShared(&nears->sectors[n]->lock); // sector�� �б� ���ؼ� ���� ��� 
			for (auto& actor_iter : nears->sectors[n]->actors)
			{
				//if (actor_iter.second->status != ST_ACTIVE) 		//continue;
				if (actor_iter.second->is_near(this) == true) // �� ��ó�� �´ٸ� ����.
				{
					is_isolated = false;
					local_new_view_list.insert(actor_iter);
				}
			}
			ReleaseSRWLockShared(&nears->sectors[n]->lock);
		}

		for (auto& new_actor : local_new_view_list)
		{
			if (0 == local_old_view_list.count(new_actor.first))	// �̵��� ���� ���̴� ����.
			{
				this->update_entering_actor(new_actor.second);		// ���� ���̴� ���� ������ NPC���� ������Ʈ ��.

				AcquireSRWLockShared(&new_actor.second->lock);
				if (0 == new_actor.second->view_list_for_npc.count(this->id)) // Ÿ �����忡�� �þ� ó�� �� �Ȱ��.
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_enter_packet(this);
				}
				else // ó�� �Ȱ��
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_move_packet(this);   // ��� �þ� ����Ʈ�� ���� �ִ� ��� �丮��Ʈ�� ������Ʈ �Ѵ�.
				}
			}
			else  // ���� �丮��Ʈ�� �ִ� ������ 
			{
				AcquireSRWLockShared(&new_actor.second->lock);					//
				if (0 != new_actor.second->view_list_for_npc.count(this->id))	// 
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_move_packet(this);
				}
				else	// �̹� ���� ���.
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_enter_packet(this);
				}
			}

			if (true == is_in_attack_range(new_actor.second))
			{
				set_target(new_actor.second);
			}
		}


		//�þ߿��� ��� �÷��̾�
		for (auto& old_it : local_old_view_list)
		{
			if (0 == local_new_view_list.count(old_it.first))		// ���� �� �þ߿� ���� �÷��̾�.
			{														// 
				this->update_leaving_actor(old_it.second);			// npc �þ߿��� ������ �������� ���� ����.

				AcquireSRWLockShared(&old_it.second->lock);			// 
				if (0 != old_it.second->view_list_for_npc.count(this->id)) // Ŭ�� �þ߿� ���� �ִ� ���
				{
					ReleaseSRWLockShared(&old_it.second->lock);
					old_it.second->send_leave_packet(this);
				}
				else	// �̹� �ٸ� �����忡�� ������ ���.
				{
					ReleaseSRWLockShared(&old_it.second->lock);
				}
			}
		}

		if (is_isolated == true)
		{
			is_active = NPC_SLEEP;
			//target = nullptr;
			has_target = false;
			// ���� Ȯ�ο� �۾� // ���۾��߿� ���� 
		}
		else // �ִٸ� ������Ʈ ���.
		{
			local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
		}
	}
	else
	{
		is_active = NPC_SLEEP;
		//target = nullptr;
		has_target = false;
	}
}

void MMONPC::update_for_fixed_combat()
{
	if (false == this->is_alive)
	{
		return;
	}

	MMOActor* local_target = this->target;
	bool	  local_has_target = this->has_target;
	if (false == has_target || false == is_in_attack_range(local_target)) // ����� ���ų� �Ÿ��� �־����� ����.
	{
		is_active = NPC_SLEEP;
		has_target = false;

		return;
	}

	if (nullptr != local_target && true == local_target->is_alive)			// Ÿ���� �����ÿ��� �� ã�ƿ�.
	{
		bool is_isolated = true;
		int local_actor_id = id;

		int temp_local_y;
		int temp_local_x;
		int local_y = temp_local_y = y;
		int local_x = temp_local_x = x;

		int temp_local_target_y;
		int temp_local_target_x;
		int local_target_y = temp_local_target_y = target->y;
		int local_target_x = temp_local_target_x = target->x;



		bool has_obstacle = true;
		for (;;) // ���� ��ο� ��ֹ��� ���ٸ� �׳�   		// �ִ� ��� ã��.
		{
			if (temp_local_x > temp_local_target_x) // x ��ǥ
			{
				temp_local_x -= 1;
			}
			else if (temp_local_x < temp_local_target_x)
			{
				temp_local_x += 1;
			}

			if (temp_local_y > temp_local_target_y) // y ��ǥ
			{
				temp_local_y -= 1;
			}
			else if (temp_local_y < temp_local_target_y)
			{
				temp_local_y += 1;
			}

			if (c2::global::obstacle_table[(temp_local_y * c2::constant::MAP_HEIGHT) + temp_local_x]) // �߰��� ��ֹ��� �ִٸ�?
			{
				break; // ���� -> astr�� ��ã��.
			}

			if (temp_local_x == temp_local_target_x && temp_local_y == temp_local_target_y) // ���� ���� ��޹��� ��������?
			{
				if (local_x > local_target_x) // x ��ǥ
				{
					local_x -= 1;
				}
				else if (local_x < local_target_x)
				{
					local_x += 1;
				}

				if (local_y > local_target_y) // y ��ǥ
				{
					local_y -= 1;
				}
				else if (local_y < local_target_y)
				{
					local_y += 1;
				}

				has_obstacle = false;

				if (local_x != local_target_x || local_y != local_target_y)
				{
					x = local_x;
					y = local_y;

					break;
				}
				else
				{
					local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
					return;
				}
			}
		}

		if (true == has_obstacle)  // ��ֹ��� �ִٸ� astar�� �� ã��.
		{
			if (true == PathFindingHelper->NewPath(local_x, local_y, local_target_x, local_target_y))
			{
				if (true == PathFindingHelper->PathNextNode())
				{
					x = local_x = PathFindingHelper->NodeGetX();
					y = local_y = PathFindingHelper->NodeGetY();
				}
			}
		}

		MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list �ܾ����.
		// ���Ͱ� ����Ǹ� �ٲٱ�.
		if (current_sector != new_sector)
		{
			AcquireSRWLockExclusive(&current_sector->lock); // ���� ���� ������
			current_sector->npcs.erase(id);
			ReleaseSRWLockExclusive(&current_sector->lock);

			// ���� ���� �鰡��.
			AcquireSRWLockExclusive(&new_sector->lock);
			this->current_sector = new_sector;
			new_sector->npcs.insert(id);
			ReleaseSRWLockExclusive(&new_sector->lock);
		}


		// �丮��Ʈ ����.
		AcquireSRWLockShared(&lock); // read �� �ɰ�...
		std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
		ReleaseSRWLockShared(&lock);


		// ���� ���ķ� ����ϸ�... 

		// �ֺ��� �Ѹ���.
		MMOSector* current_sector = new_sector;
		const MMONear* nears = current_sector->get_near(local_y, local_x);
		int				near_cnt = nears->count;


		// �� �ֺ� ������ �ܾ� ����.
		std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
		for (int n = 0; n < near_cnt; ++n)
		{
			AcquireSRWLockShared(&nears->sectors[n]->lock); // sector�� �б� ���ؼ� ���� ��� 
			for (auto& actor_iter : nears->sectors[n]->actors)
			{
				//if (actor_iter.second->status != ST_ACTIVE) 		//continue;
				if (actor_iter.second->is_near(this) == true) // �� ��ó�� �´ٸ� ����.
				{
					is_isolated = false;
					local_new_view_list.insert(actor_iter);
				}
			}
			ReleaseSRWLockShared(&nears->sectors[n]->lock);
		}

		for (auto& new_actor : local_new_view_list)
		{
			if (0 == local_old_view_list.count(new_actor.first))	// �̵��� ���� ���̴� ����.
			{
				this->update_entering_actor(new_actor.second);		// ���� ���̴� ���� ������ NPC���� ������Ʈ ��.

				AcquireSRWLockShared(&new_actor.second->lock);
				if (0 == new_actor.second->view_list_for_npc.count(this->id)) // Ÿ �����忡�� �þ� ó�� �� �Ȱ��.
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_enter_packet(this);
				}
				else // ó�� �Ȱ��
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_move_packet(this);   // ��� �þ� ����Ʈ�� ���� �ִ� ��� �丮��Ʈ�� ������Ʈ �Ѵ�.
				}
			}
			else  // ���� �丮��Ʈ�� �ִ� ������ 
			{
				AcquireSRWLockShared(&new_actor.second->lock);					//
				if (0 != new_actor.second->view_list_for_npc.count(this->id))	// 
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_move_packet(this);
				}
				else	// �̹� ���� ���.
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_enter_packet(this);
				}
			}

			if (true == is_in_attack_range(new_actor.second))
			{
				set_target(new_actor.second);
			}
		}


		//�þ߿��� ��� �÷��̾�
		for (auto& old_it : local_old_view_list)
		{
			if (0 == local_new_view_list.count(old_it.first))		// ���� �� �þ߿� ���� �÷��̾�.
			{														// 
				this->update_leaving_actor(old_it.second);			// npc �þ߿��� ������ �������� ���� ����.

				AcquireSRWLockShared(&old_it.second->lock);			// 
				if (0 != old_it.second->view_list_for_npc.count(this->id)) // Ŭ�� �þ߿� ���� �ִ� ���
				{
					ReleaseSRWLockShared(&old_it.second->lock);
					old_it.second->send_leave_packet(this);
				}
				else	// �̹� �ٸ� �����忡�� ������ ���.
				{
					ReleaseSRWLockShared(&old_it.second->lock);
				}
			}
		}

		if (is_isolated == true)
		{
			is_active = NPC_SLEEP;
			//target = nullptr;
			has_target = false;
			// ���� Ȯ�ο� �۾� // ���۾��߿� ���� 
			//local_timer->push_timer_task(this->id, TTT_ON_SLEEP, 1000, 0);
		}
		else // �ִٸ� ������Ʈ ���.
		{
			local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
		}
	}
	else
	{
		is_active	= NPC_SLEEP;
		//target	= nullptr;
		has_target	= false;
	}
}

void MMONPC::update_for_combat()
{
	if (false == this->is_alive)
	{
		return;
	}

	bool is_isolated = true;
	int local_y = y;
	int local_x = x;
	int local_actor_id = id;

	MMOActor* local_target = this->target;
	

	if (nullptr != local_target && true == local_target->is_alive)			// Ÿ���� �����ÿ��� �� ã�ƿ�.
	{
		bool is_isolated = true;
		int local_actor_id = id;

		int temp_local_y;
		int temp_local_x;
		int local_y = temp_local_y = y;
		int local_x = temp_local_x = x;

		int temp_local_target_y;
		int temp_local_target_x;
		int local_target_y = temp_local_target_y = target->y;
		int local_target_x = temp_local_target_x = target->x;



		bool has_obstacle = true;
		for (;;) // ���� ��ο� ��ֹ��� ���ٸ� �׳�   		// �ִ� ��� ã��.
		{
			if (temp_local_x > temp_local_target_x) // x ��ǥ
			{
				temp_local_x -= 1;
			}
			else if (temp_local_x < temp_local_target_x)
			{
				temp_local_x += 1;
			}

			if (temp_local_y > temp_local_target_y) // y ��ǥ
			{
				temp_local_y -= 1;
			}
			else if (temp_local_y < temp_local_target_y)
			{
				temp_local_y += 1;
			}

			if (c2::global::obstacle_table[(temp_local_y * c2::constant::MAP_HEIGHT) + temp_local_x]) // �߰��� ��ֹ��� �ִٸ�?
			{
				break; // ���� -> astr�� ��ã��.
			}

			if (temp_local_x == temp_local_target_x && temp_local_y == temp_local_target_y) // ���� ���� ��޹��� ��������?
			{
				if (local_x > local_target_x) // x ��ǥ
				{
					local_x -= 1;
				}
				else if (local_x < local_target_x)
				{
					local_x += 1;
				}

				if (local_y > local_target_y) // y ��ǥ
				{
					local_y -= 1;
				}
				else if (local_y < local_target_y)
				{
					local_y += 1;
				}

				has_obstacle = false;

				if (local_x != local_target_x || local_y != local_target_y)
				{
					x = local_x;
					y = local_y;

					break;
				}
				else
				{
					local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
					return;
				}
			}
		}

		if (true == has_obstacle)  // ��ֹ��� �ִٸ� astar�� �� ã��.
		{
			if (true == PathFindingHelper->NewPath(local_x, local_y, local_target_x, local_target_y))
			{
				if (true == PathFindingHelper->PathNextNode())
				{
					x = local_x = PathFindingHelper->NodeGetX();
					y = local_y = PathFindingHelper->NodeGetY();
				}
			}
		}

		MMOSector* new_sector = g_zone->get_sector(local_y, local_x);			// view_list �ܾ����.
		// ���Ͱ� ����Ǹ� �ٲٱ�.
		if (current_sector != new_sector)
		{
			AcquireSRWLockExclusive(&current_sector->lock); // ���� ���� ������
			current_sector->npcs.erase(id);
			ReleaseSRWLockExclusive(&current_sector->lock);

			// ���� ���� �鰡��.
			AcquireSRWLockExclusive(&new_sector->lock);
			this->current_sector = new_sector;
			new_sector->npcs.insert(id);
			ReleaseSRWLockExclusive(&new_sector->lock);
		}


		// �丮��Ʈ ����.
		AcquireSRWLockShared(&lock); // read �� �ɰ�...
		std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
		ReleaseSRWLockShared(&lock);


		// ���� ���ķ� ����ϸ�... 

		// �ֺ��� �Ѹ���.
		MMOSector* current_sector = new_sector;
		const MMONear* nears = current_sector->get_near(local_y, local_x);
		int				near_cnt = nears->count;


		// �� �ֺ� ������ �ܾ� ����.
		std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
		for (int n = 0; n < near_cnt; ++n)
		{
			AcquireSRWLockShared(&nears->sectors[n]->lock); // sector�� �б� ���ؼ� ���� ��� 
			for (auto& actor_iter : nears->sectors[n]->actors)
			{
				//if (actor_iter.second->status != ST_ACTIVE) 		//continue;
				if (actor_iter.second->is_near(this) == true) // �� ��ó�� �´ٸ� ����.
				{
					is_isolated = false;
					local_new_view_list.insert(actor_iter);
				}
			}
			ReleaseSRWLockShared(&nears->sectors[n]->lock);
		}

		for (auto& new_actor : local_new_view_list)
		{
			if (0 == local_old_view_list.count(new_actor.first))	// �̵��� ���� ���̴� ����.
			{
				this->update_entering_actor(new_actor.second);		

				AcquireSRWLockShared(&new_actor.second->lock);
				if (0 == new_actor.second->view_list_for_npc.count(this->id)) // Ÿ �����忡�� �þ� ó�� �� �Ȱ��.
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_enter_packet(this);
				}
				else // ó�� �Ȱ��
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_move_packet(this);   // ��� �þ� ����Ʈ�� ���� �ִ� ��� �丮��Ʈ�� ������Ʈ �Ѵ�.
				}
			}
			else  // ���� �丮��Ʈ�� �ִ� ������ 
			{
				AcquireSRWLockShared(&new_actor.second->lock);					//
				if (0 != new_actor.second->view_list_for_npc.count(this->id))	// 
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_move_packet(this);
				}
				else	// �̹� ���� ���.
				{
					ReleaseSRWLockShared(&new_actor.second->lock);

					new_actor.second->send_enter_packet(this);
				}
			}

			if (true == is_in_attack_range(new_actor.second))
			{
				set_target(new_actor.second);
			}
		}


		//�þ߿��� ��� �÷��̾�
		for (auto& old_it : local_old_view_list)
		{
			if (0 == local_new_view_list.count(old_it.first))		// ���� �� �þ߿� ���� �÷��̾�.
			{														// 
				this->update_leaving_actor(old_it.second);			// npc �þ߿��� ������ �������� ���� ����.

				AcquireSRWLockShared(&old_it.second->lock);			// 
				if (0 != old_it.second->view_list_for_npc.count(this->id)) // Ŭ�� �þ߿� ���� �ִ� ���
				{
					ReleaseSRWLockShared(&old_it.second->lock);
					old_it.second->send_leave_packet(this);
				}
				else	// �̹� �ٸ� �����忡�� ������ ���.
				{
					ReleaseSRWLockShared(&old_it.second->lock);
				}
			}
		}

		if (is_isolated == true)
		{
			is_active = NPC_SLEEP;
			// ���� Ȯ�ο� �۾� // ���۾��߿� ���� 
			//local_timer->push_timer_task(this->id, TTT_ON_SLEEP, 1000, 0);
		}
		else // �ִٸ� ������Ʈ ���.
		{
			local_timer->push_timer_task(this->id, TTT_UPDATE_FOR_NPC, 1000, 0);
		}
	}
	else
	{
		is_active = NPC_SLEEP;
		has_target = false;
	}
}

void MMONPC::set_target(MMOActor* actor)
{
	if (false == has_target && false == InterlockedExchange8((volatile char*)&has_target, true))
	{
		printf("%d target : %d \n", this->type, actor->session_id);
		this->target = actor;
	}
}


void MMONPC::decrease_hp(MMOActor* actor, int32_t damage)
{
	AcquireSRWLockExclusive(&this->lock);
	// ���� �ϰ� NPC �׾����� �׾��ٰ� ���� �ٲٱ� ����;

	
	hp -= damage;
	if (hp <= 0)
	{
		if ( this->is_alive != false && true == InterlockedExchange8((volatile CHAR*)&is_alive, 0))// = false; // ���� �ϰ� NPC �׾����� �׾��ٰ� ���� �ٲٱ� ����
		{
			AcquireSRWLockExclusive(&current_sector->lock); // ���� ���� ������. // �ٸ� Ŭ������ �ϱ� �����..
			current_sector->npcs.erase(id);
			ReleaseSRWLockExclusive(&current_sector->lock);

			actor->increase_exp(exp);// ���ΰŸ� ����ġ�� ��� �׿� ���� ó���� �Ѵ�.

			for (auto& iter : view_list)	// npc�� �ֺ��� ��ε� ĳ������. �׾ �����ٰ� 
			{
				MMOActor* neighbor = iter.second;

				neighbor->send_leave_packet(this); // �� �þ߸���Ʈ �÷��̾�� �����ٰ� �˸�.
			}

			ReleaseSRWLockExclusive(&this->lock); // ���⼱ ���� Ǭ��.
			
			send_chat_packet_to_target(actor, system_msg_hit);

			local_timer->push_timer_task(id, TTT_RESPAWN_FOR_NPC, 30'000, 0);
		}
		else
		{
			ReleaseSRWLockExclusive(&this->lock);

			send_chat_packet_to_target(actor, system_msg_hit);
			return;
		}
	}
	else
	{
		send_chat_packet_to_target(actor, system_msg_hit);
		// ���������� Ÿ�̸ӿ� 30���� ������ �̺�Ʈ�� �߰�.
		//else  // ���� �������ݻ� ü�� ���̴°� �Ⱦ˷��൵ �ȴ�.//{//}
		ReleaseSRWLockExclusive(&this->lock); // ���⼱ ���� Ǭ��.
		return;
	}
}

bool MMONPC::is_in_attack_range(MMOActor* actor)
{
	if (abs(this->x - actor->x) > FOV_HALF_WIDTH_FOR_COMBAT_MOB) return false;
	if (abs(this->y - actor->y) > FOV_HALF_HEIGHT_FOR_COMBAT_MOB) return false;

	return true;
}

void MMONPC::send_chat_packet(const wchar_t* msg)
{
	AcquireSRWLockShared(&lock); // ���Ϳ� npc�� ���� �б� �۾���.
	std::unordered_map<int32_t, MMOActor*> local_view_list = view_list;		// �ӽ� �丮��Ʈ.
	ReleaseSRWLockShared(&lock);

	sc_packet_chat chat_payload; 	// id check
	chat_payload.header.type = S2C_CHAT;
	chat_payload.header.length = sizeof(sc_packet_chat);
	wcsncpy_s(chat_payload.chat, msg, c2::constant::MAX_CHAT_LEN);


	c2::Packet* out_packet = c2::Packet::alloc();
	out_packet->write(&chat_payload, sizeof(sc_packet_chat));
	out_packet->add_ref(local_view_list.size()); 

	for (auto& actor_it : local_view_list)
	{
		g_server->send_packet(actor_it.second->session_id, out_packet);	
	}

	out_packet->decrease_ref_count();
}

void MMONPC::send_chat_packet_to_target(MMOActor* target, const wchar_t* msg)
{
	sc_packet_chat chat_payload; 	// id check
	chat_payload.header.type = S2C_CHAT;
	chat_payload.header.length = sizeof(sc_packet_chat);
	wcsncpy_s(chat_payload.chat, msg, c2::constant::MAX_CHAT_LEN);

	c2::Packet* out_packet = c2::Packet::alloc();
	out_packet->write(&chat_payload, sizeof(sc_packet_chat));

	g_server->send_packet(target->session_id, out_packet);
}


void MMONPC::update_entering_actor(MMOActor* other)
{
	AcquireSRWLockExclusive(&this->lock);
	this->view_list.emplace(other->get_id(), other);
	ReleaseSRWLockExclusive(&this->lock);
}

void MMONPC::update_leaving_actor(MMOActor* actor)
{
	AcquireSRWLockExclusive(&this->lock);
	this->view_list.erase(actor->get_id());
	ReleaseSRWLockExclusive(&this->lock);
}
