
#include "pre_compile.h"
#include "enviroment.h"
#include "function.h"
#include "MMONpc.h"
#include "MMOActor.h"
#include "MMONpcManager.h"
#include "MMOZone.h"
#include "MMONear.h"
#include "MMOServer.h"

//#include "../C2Server/C2Server/util/TimeScheduler.h"
//#include "MMOActor.h"

using namespace c2::constant;
#include "script_api.h"

void MMONpc::prepare_virtual_machine()
{
	lua_vm = luaL_newstate();
	luaL_openlibs(lua_vm);

	int error = luaL_loadfile(lua_vm, "npc.lua");
	if (error != 0)
		printf("1lua error %s \n", lua_tostring(lua_vm, -1));
	error = lua_pcall(lua_vm, 0, 0, 0);
	if (error != 0) 
		printf( "2lua error %s \n", lua_tostring(lua_vm, -1));

	// 
	lua_getglobal(lua_vm, "prepare_npc_script");
	lua_pushnumber(lua_vm, (int32_t)id);
	lua_pcall(lua_vm, 1, 0, 0);


	lua_register(lua_vm, "server_send_chatting",		l2c_send_chatting_to_target);
	lua_register(lua_vm, "server_get_npc_x",			l2c_get_npc_pos_x);
	lua_register(lua_vm, "server_get_npc_y",			l2c_get_npc_pos_y);
	lua_register(lua_vm, "server_npc_move_to_anywhere", l2c_npc_move_to_anywhere);
	lua_register(lua_vm, "server_npc_go_sleep",			l2c_npc_go_sleep);
}

void MMONpc::move()
{
	bool is_isolated = true;
	int local_y = y;
	int local_x = x;
	int local_actor_id = id;



	// ��ֹ� üũ ���.
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



	MMOSector* new_sector = zone->get_sector(local_y, local_x);			// view_list �ܾ����.
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
	std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list;
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

	// �ֺ��� �ƹ��� ���ٸ�?
	if (is_isolated == true)
	{
		// �� ����� �ǵ鿴���� �������� ó������.
		// Ÿ �����忡�� �����ؼ� ���� ���� ç �� �ֵ���...
		is_active = NPC_SLEEP;
	}
	else // �ִٸ� ������Ʈ ���.
	{
		// is_active ������ �� ������ �־ ������Ʈ ������ ���� ������ ���� ����.
		//if (InterlockedExchange(&this->is_active, NPC_WORKING) == NPC_SLEEP) 
		local_timer->push_timer_task(this->id, TTT_MOVE_NPC, 1000, 0);
	}
}

void MMONpc::move_to_anywhere()
{
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

	MMOSector* new_sector = zone->get_sector(local_y, local_x);			// view_list �ܾ����.
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
	std::unordered_map<int32_t, MMOActor*> local_old_view_list = view_list;
	ReleaseSRWLockExclusive(&lock); // // �� �ɰ�...


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

	AcquireSRWLockExclusive(&vm_lock);
	lua_getglobal(this->lua_vm, "update_script");
	this->is_active = NPC_SLEEP;
	lua_pcall(this->lua_vm, 0, 0, 0);
	ReleaseSRWLockExclusive(&vm_lock);
}
	
void MMONpc::send_chatting_to_actor(int32_t actor_id ,char* message)
{
	sc_packet_chat chat_payload;
	chat_payload.header.length = sizeof(sc_packet_chat);
	chat_payload.header.type = S2C_CHAT;
	chat_payload.id = this->id;
	strcpy(chat_payload.chat, message);
	
	c2::Packet* chat_packet = c2::Packet::alloc();
	chat_packet->write(&chat_payload, sizeof(sc_packet_chat));

	zone->server->send_packet(actor_id, chat_packet);
}

void MMONpc::update_entering_actor(MMOActor* other)
{
	AcquireSRWLockExclusive(&this->lock);
	this->view_list.emplace(other->get_id(), other);
	ReleaseSRWLockExclusive(&this->lock);
}

void MMONpc::update_leaving_actor(MMOActor* actor)
{
	AcquireSRWLockExclusive(&this->lock);
	this->view_list.erase(actor->get_id());
	ReleaseSRWLockExclusive(&this->lock);
}
