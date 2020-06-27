
#include "pre_compile.h"
#include "MMOServer.h"
#include "mmo_function.hpp"
#include "MMONPC.h"
#include "MMOActor.h"
#include "MMONpcManager.h"
#include "MMOZone.h"
#include "MMONear.h"


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
	this->x =		lua_tonumber(lua_vm, -6);
	this->y =		lua_tonumber(lua_vm, -5);
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
	AcquireSRWLockExclusive(&lock); // // �� �ɰ�...
	std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list; // deep copy 
	ReleaseSRWLockExclusive(&lock); // // �� �ɰ�...


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
	is_active = 0;
}

void MMONPC::reset()
{
	current_sector = nullptr;
	hp = 200;
	max_hp = 200;

	target = nullptr;

	this->view_list.clear();

	//x = rand() % c2::constant::MAP_WIDTH;
	//y = rand() % c2::constant::MAP_HEIGHT;

	zone = this->zone;
	is_alive = true;
	is_active = 0;
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


void MMONPC::decrease_hp(MMOActor* actor, int32_t damage)
{
	AcquireSRWLockExclusive(&this->lock);
	// ���� �ϰ� NPC �׾����� �׾��ٰ� ���� �ٲٱ� ����;
	if (this->is_alive == false) // ���� ���¸�  ���� �ǵ��� ���� �������.
	{
		ReleaseSRWLockExclusive(&this->lock);
		return;
	}
	
	hp -= damage;
	if (hp <= 0)
	{
		is_active = false; // ���� �ϰ� NPC �׾����� �׾��ٰ� ���� �ٲٱ� ����
		
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

		local_timer->push_timer_task(id, TTT_RESPAWN_FOR_NPC, 30'000, 0);
	}
	else
	{
		// ���������� Ÿ�̸ӿ� 30���� ������ �̺�Ʈ�� �߰�.
		//else  // ���� �������ݻ� ü�� ���̴°� �Ⱦ˷��൵ �ȴ�.//{//}
		ReleaseSRWLockExclusive(&this->lock); // ���⼱ ���� Ǭ��.
	}
}

bool MMONPC::is_near(MMOActor* actor)
{
	
	return false;
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
