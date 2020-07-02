#include "core/packet_handler.h"
//#include "core/OuterServer.h"
#include "MMOSession.h"
#include "MMOServer.h"
#include "MMOZone.h"
#include "MMONpcManager.h"
#include "DBManager.h"

#include <unordered_map>
#include <unordered_set>

REGISTER_HANDLER(C2S_LOGIN)
{
	MMOSession* mmo_session		= (MMOSession*)session;
	MMOActor*	my_actor		= mmo_session->get_actor();
	MMOServer*	mmo_server		= (MMOServer*)session->server;
	my_actor->zone				= mmo_server->get_zone();
	my_actor->server			= mmo_server;

////////////////////////////////// ���� ������ Ŭ���̾�Ʈ ������Ʈ.
	cs_packet_login login_payload;
	in_packet.read(&login_payload, sizeof(cs_packet_login));
	
	my_actor->reset();

	memcpy(my_actor->name, login_payload.name, c2::constant::MAX_ID_LEN * 2); // ������ ���⼭�� write ��. and ���� ��;;

	if (my_actor->get_id() != (uint16_t)session->session_id) // �̹� ������ ���� �༮.
	{
		printf("Session::invalid session id %d\n", my_actor->session_id );

		mmo_session->request_disconnection();

		return;
	}

	mmo_session->request_login_validation(login_payload.name);

	return;
}



REGISTER_HANDLER(C2S_MOVE)
{
	MMOSession* mmo_session = (MMOSession*)session;
	MMOActor*	my_actor	= mmo_session->get_actor();
	MMOServer*	mmo_server	= (MMOServer*)session->server;
	MMOZone*	mmo_zone	= mmo_server->get_zone();

	MMONpcManager* mmo_npc_mgr = g_npc_manager;

	cs_packet_move cs_move_payload;								//	���� �̵�����.
	in_packet.read(&cs_move_payload, sizeof(cs_packet_move));	// 

	if (my_actor->is_alive == false)
		return;

	// �纻 �����.
	int local_y = my_actor->y;
	int local_x = my_actor->x;
	int local_actor_id = my_actor->get_id();


	// ��ֹ� üũ ���.
	switch (cs_move_payload.direction)
	{
	case c2::constant::D_DOWN: 
		if (local_y < MAP_HEIGHT -1)	local_y++; 
		break;
	case c2::constant::D_LEFT:
		if (local_x > 0) local_x--;
		break;
	case c2::constant::D_RIGHT:
		if (local_x < MAP_WIDTH - 1) local_x++;
		break;
	case c2::constant::D_UP:
		if (local_y > 0) local_y--;
		break;
	default:
		size_t* invalid_ptr{}; *invalid_ptr = 02;
		break;
	}

	if (true == g_zone->has_obstacle(local_y, local_x))
	{
		return;
	}

	my_actor->x = local_x;
	my_actor->y = local_y;

	MMOSector* prev_sector = my_actor->current_sector;					// view_list �ܾ����.
	MMOSector* curent_sector = mmo_zone->get_sector(local_y, local_x);			// view_list �ܾ����.


	if (prev_sector != curent_sector)										//���Ͱ� �ٲ� ���.
	{
		if (nullptr != prev_sector)
		{
			AcquireSRWLockExclusive(&prev_sector->lock);						// ���� ���Ϳ��� ������ ���ؼ�.
			prev_sector->actors.erase(local_actor_id);
			ReleaseSRWLockExclusive(&prev_sector->lock);
		}

		AcquireSRWLockExclusive(&curent_sector->lock);						// �� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
		curent_sector->actors.emplace(local_actor_id, my_actor);
		my_actor->current_sector = curent_sector;							// Ÿ �����忡�� �����ϸ� ���� �Ϸ� �ϰ�?
		ReleaseSRWLockExclusive(&curent_sector->lock);						// �� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 

		mmo_session->request_updating_position(local_y, local_x);
	}


	AcquireSRWLockShared(&my_actor->lock);
	std::unordered_map<int32_t, MMOActor*> local_old_view_list = my_actor->view_list;
	std::unordered_set<int32_t> local_old_view_list_for_npc = my_actor->view_list_for_npc;
	ReleaseSRWLockShared(&my_actor->lock);



	/// ���� ���ϱ�...
	MMOSector*		current_sector	= curent_sector;
	const MMONear*	nears			= current_sector->get_near(local_y, local_x);
	int				near_cnt		= nears->count;


	// �� �ֺ� ������ �ܾ� ����.
	std::unordered_map<int32_t, MMOActor*>	local_new_view_list;
	std::unordered_set<int32_t>				local_new_view_list_for_npc;
	for (int n = 0; n < near_cnt; ++n)					
	{
		AcquireSRWLockShared(&nears->sectors[n]->lock); // sector�� �б� ���ؼ� ���� ��� 
		for (auto& actor_iter : nears->sectors[n]->actors)
		{
			//if (actor_iter.second->status != ST_ACTIVE) 
				//continue;
			if (actor_iter.second == my_actor)
				continue;

			if (my_actor->is_near(actor_iter.second) == true) // �� ��ó�� �´ٸ� ����.
				local_new_view_list.insert(actor_iter);
		}

		for (auto npc_id : nears->sectors[n]->npcs)
		{
			//if (actor_iter.second->status != ST_ACTIVE) 
				//continue;
			MMONPC* npc = mmo_npc_mgr->get_npc(npc_id);
			if (my_actor->is_near(npc) == true) // �� ��ó�� �´ٸ� ����.
			{
				my_actor->wake_up_npc(npc);	// ���δ� ����� ������ �ֱ�?
				//local_timer->push_timer_task(npc->id, TTT_ON_WAKE_FOR_NPC, 1, my_actor->session_id);
				local_new_view_list_for_npc.insert(npc_id);
			}
		}
		ReleaseSRWLockShared(&nears->sectors[n]->lock);
	}
	
	
	
	c2::Packet*			my_move_packet = c2::Packet::alloc();
	sc_packet_move		sc_move_payload{ {sizeof(sc_packet_move), S2C_MOVE}, local_actor_id, local_x, local_y, cs_move_payload.move_time };

	my_move_packet->write(&sc_move_payload, sizeof(sc_packet_move));  // ������ �� �̵�����.
	mmo_server->send_packet(my_actor->session_id, my_move_packet);


///////////////////
	for (auto& new_actor : local_new_view_list)
	{
		if( 0 == local_old_view_list.count(new_actor.first) ) // �̵��� ���� ���̴� ����.
		{
			my_actor->send_enter_packet(new_actor.second);		// Ÿ�� ������ ������ ����.

			AcquireSRWLockShared(&new_actor.second->lock);							
			if ( 0 == new_actor.second->view_list.count(my_actor->get_id()) ) // Ÿ �����忡�� �þ� ó�� �� �Ȱ��.
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_enter_packet(my_actor);
			}
			else // ó�� �Ȱ��
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_move_packet(my_actor);   // ��� �þ� ����Ʈ�� ���� �ִ� ��� �丮��Ʈ�� ������Ʈ �Ѵ�.
			}
		}
		else  // ���� �丮��Ʈ�� �ִ� ������ 
		{
			AcquireSRWLockShared(&new_actor.second->lock);					//
			if (0 != new_actor.second->view_list.count(my_actor->get_id()))	// 
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_move_packet(my_actor);
			}
			else	// �̹� ���� ���.
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_enter_packet(my_actor);
			}
		}
	}

	//�þ߿��� ��� �÷��̾�
	for (auto& old_it : local_old_view_list)
	{
		if (0 == local_new_view_list.count(old_it.first))
		{
			my_actor->send_leave_packet(old_it.second);

			AcquireSRWLockShared(&old_it.second->lock);
			if (0 != old_it.second->view_list.count(my_actor->get_id()))
			{
				ReleaseSRWLockShared(&old_it.second->lock);
				old_it.second->send_leave_packet(my_actor);
			}
			else	// �̹� �ٸ� �����忡�� ������ ���.
			{
				ReleaseSRWLockShared(&old_it.second->lock);
			}
		}
	}


// ������ npc���� �ϴ� ���� �߰�.
	for (int32_t npc_id : local_new_view_list_for_npc)
	{
		MMONPC* npc = mmo_npc_mgr->get_npc(npc_id);


		if (0 == local_old_view_list_for_npc.count(npc_id)) // �̵��� ���� ���̴� NPC
		{
			my_actor->send_enter_packet(npc); // �� npc ������ ������ ����.

			AcquireSRWLockExclusive(&npc->lock);
			npc->view_list.emplace(my_actor->get_id(), my_actor);
			ReleaseSRWLockExclusive(&npc->lock);
		}

		if (NT_COMBAT_FIXED <= npc->type && true == npc->is_near(my_actor)) // �Ĺ� ���� ������� �õ� �غ�.
		{
			if (npc->has_target == false)
			{
				printf("[%d] %d  \n", npc->type, npc->id);
			}

			npc->set_target(my_actor);							
		}
	}

// ���� �þ߿� �־��� �̵��� ������ �༮�� ����.
	for (int32_t old_npc_id : local_old_view_list_for_npc)
	{
		if (0 == local_new_view_list_for_npc.count(old_npc_id))
		{
			//AcquireSRWLockExclusive(&my_actor->lock);
			//my_actor->view_list_for_npc.erase(old_npc_id);
			//ReleaseSRWLockExclusive(&my_actor->lock);
			// �����°� npc�� �˾Ƽ� ����. // ��� 1�ʿ� �ѹ��̱⵵ �ϰ� ���� �ݿ��� ����.
			MMONPC* npc = mmo_npc_mgr->get_npc(old_npc_id);

			my_actor->send_leave_packet(npc); // �� npc ������ ������ ����.

			AcquireSRWLockExclusive(&npc->lock);
			npc->view_list.erase(my_actor->get_id());
			ReleaseSRWLockExclusive(&npc->lock);
		}
	}
}


REGISTER_HANDLER(C2S_ATTACK)
{
	MMOSession*		mmo_session	{ (MMOSession*)session };
	MMOActor*		my_actor	{ mmo_session->get_actor() };

	if (my_actor->is_alive == false)
		return;

	my_actor->attack();
}

// �丮��Ʈ �ݿ��� ������ ���� ������ �غ����ҵ�.
REGISTER_HANDLER(C2S_CHAT)
{
	MMOSession* mmo_session		{ (MMOSession*)session };
	MMOActor*	my_actor		{ mmo_session->get_actor() };
	MMOServer*	mmo_server		{ (MMOServer*)session->server };
	MMOSector*	sector			{ my_actor->current_sector };

	cs_packet_chat chat_payload; 								// id check
	in_packet.read(&chat_payload, sizeof(cs_packet_chat));


	AcquireSRWLockShared(&my_actor->lock); // ���Ϳ� npc�� ���� �б� �۾���.
	std::unordered_map<int32_t, MMOActor*> local_view_list = my_actor->view_list;		// �ӽ� �丮��Ʈ.
	ReleaseSRWLockShared(&my_actor->lock);
	//local_view_list.clear();									// ??? �Ƹ� ����ƽ���� ����� �׽�Ʈ�� ���غ���.

	// ���������� ������ �Ȱ��� Ÿ�Ը� �ٲ��ָ� ��.
	c2::Packet* out_packet = c2::Packet::alloc();
	chat_payload.header.type = S2C_CHAT;
	chat_payload.header.length = sizeof(cs_packet_chat);
	out_packet->write(&chat_payload, sizeof(cs_packet_chat));	//���Ⱑ �� �����ΰ�;
	
	out_packet->add_ref( local_view_list.size() + 1); // ������ 

	for (auto& actor_it : local_view_list)
	{
		mmo_server->send_packet(actor_it.second->session_id, out_packet);	// 
	}


	mmo_server->send_packet(my_actor->session_id, out_packet);	// 

	out_packet->decrease_ref_count();
}


REGISTER_HANDLER(C2S_LOGOUT)
{
	MMOSession* mmo_session = (MMOSession*)session;
	MMOActor* my_actor = mmo_session->get_actor();
	MMOServer* mmo_server = (MMOServer*)session->server;
	MMOZone* mmo_zone = mmo_server->get_zone();

	//my_actor->session->request_updating_position(my_actor->y, my_actor->x); // ���� �� DB ����.
	my_actor->session->request_updating_all(my_actor->y, my_actor->x, my_actor->hp, my_actor->level, my_actor->current_exp); // �߰� ����..

	int			my_actor_id = my_actor->get_id(); // 

	MMOSector* my_actor_sector = my_actor->current_sector; 
	if (my_actor_sector == nullptr)							// 
	{
		return;
	}


	AcquireSRWLockExclusive(&my_actor->lock);				// �� ��.

	AcquireSRWLockExclusive(&my_actor_sector->lock);		//  view lsit �� ���� .
	my_actor_sector->actors.erase(my_actor_id);
	ReleaseSRWLockExclusive(&my_actor_sector->lock);

	auto& view_list = my_actor->view_list;					// 
	for (auto& actor_iter : view_list)
	{
		if (actor_iter.second == my_actor)
		{
			actor_iter.second->send_leave_packet_without_updating_viewlist(my_actor); 
			continue;
		}
		//if (actor_iter.second == my_actor) continue;
		AcquireSRWLockExclusive(&actor_iter.second->lock); 
		if (0 != actor_iter.second->view_list.count(my_actor_id))
		{
			ReleaseSRWLockExclusive(&actor_iter.second->lock);
			actor_iter.second->send_leave_packet(my_actor);
		}
		else
		{
			ReleaseSRWLockExclusive(&actor_iter.second->lock);
		}
	}
	view_list.clear();

	ReleaseSRWLockExclusive(&my_actor->lock);
}

