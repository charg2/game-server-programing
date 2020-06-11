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
	MMOZone*	mmo_zone		= (MMOZone*)my_actor->zone;
	MMONpcManager* mmo_npc_mgr	= g_npc_manager;

////////////////////////////////// ���� ������ Ŭ���̾�Ʈ ������Ʈ.
	cs_packet_login login_payload;
	in_packet.read(&login_payload, sizeof(cs_packet_login));
	
	my_actor->reset();
	memcpy(my_actor->name, login_payload.name, c2::constant::MAX_ID_LEN); // ������ ���⼭�� write ��. and ���� ��;;

	if (mmo_session->get_actor()->get_id() != (uint16_t)session->session_id) // �̹� ������ ���� �༮.
	{
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
		size_t* invalid_ptr{}; *invalid_ptr = 0;
		break;
	}


	my_actor->x = local_x;
	my_actor->y = local_y;

	mmo_session->request_updating_position(local_y, local_x);

	MMOSector* prev_sector = my_actor->current_sector;					// view_list �ܾ����.
	MMOSector* curent_sector = mmo_zone->get_sector(local_y, local_x);			// view_list �ܾ����.


	if (prev_sector != curent_sector)										//���Ͱ� �ٲ� ���.
	{
		AcquireSRWLockExclusive(&curent_sector->lock);						// ���� ���Ϳ��� ������ ���ؼ�.
		prev_sector->actors.erase(local_actor_id);
		ReleaseSRWLockExclusive(&curent_sector->lock);						

		AcquireSRWLockExclusive(&curent_sector->lock);						// �� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
		curent_sector->actors.emplace(local_actor_id, my_actor);
		my_actor->current_sector = curent_sector;							// Ÿ �����忡�� �����ϸ� ���� �Ϸ� �ϰ�?
		ReleaseSRWLockExclusive(&curent_sector->lock);						// �� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
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
			MMONpc* npc = mmo_npc_mgr->get_npc(npc_id);
			if (my_actor->is_near(npc) == true) // �� ��ó�� �´ٸ� ����.
			{
				//my_actor->wake_up_npc(npc);
				local_timer->push_timer_task(npc->id, TTT_NPC_SCRIPT, 1, my_actor->session_id);

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
	///////////////////
	for (int32_t npc_id : local_new_view_list_for_npc)
	{
		if (0 == local_old_view_list_for_npc.count(npc_id)) // �̵��� ���� ���̴� NPC
		{
			MMONpc* npc = mmo_npc_mgr->get_npc(npc_id);
			
			my_actor->send_enter_packet(npc); // �� npc ������ ������ ����.

		}
	}

	for (int32_t old_npc_id : local_old_view_list_for_npc)
	{
		if (0 == local_new_view_list_for_npc.count(old_npc_id))
		{
			//AcquireSRWLockExclusive(&my_actor->lock);
			//my_actor->view_list_for_npc.erase(old_npc_id);
			//ReleaseSRWLockExclusive(&my_actor->lock);
			// �����°� npc�� �˾Ƽ� ����. // ��� 1�ʿ� �ѹ��̱⵵ �ϰ� ���� �ݿ��� ����.
			MMONpc* npc = mmo_npc_mgr->get_npc(old_npc_id);

			my_actor->send_leave_packet(npc); // �� npc ������ ������ ����.
		}
	}
}


REGISTER_HANDLER(C2S_CHAT)
{
	MMOSession* mmo_session		{ (MMOSession*)session };
	MMOActor*	my_actor		{ mmo_session->get_actor() };
	MMOServer*	mmo_server		{ (MMOServer*)session->server };
	MMOSector*	sector			{ my_actor->current_sector };

	cs_packet_chat chat_payload; 								// id check
	in_packet.read(&chat_payload, sizeof(cs_packet_chat));


	//static thread_local 

	std::unordered_map<int16_t, MMOActor*> local_view_list;
	local_view_list.clear();


	MMOSector* current_sector = mmo_server->get_zone()->get_sector(my_actor);			// view_list �ܾ����.
	const MMONear* nears = current_sector->get_near(my_actor->y, my_actor->x); // �ֺ� ���͵�.
	int near_cnt = nears->count;

	for (int i = 0; i < near_cnt; ++i)
	{
		AcquireSRWLockShared(&nears->sectors[i]->lock);

		for (auto& iter : nears->sectors[i]->actors)
		{
			MMOActor* neighbor = iter.second;
			if (my_actor->is_near(neighbor) == true) // �� ��ó�� �´ٸ� ����.
				local_view_list.emplace(iter);
		}
		
		ReleaseSRWLockShared(&nears->sectors[i]->lock);
	}
	

	c2::Packet* out_packet = c2::Packet::alloc();
	// ���������� ������ �Ȱ��� Ÿ�Ը� �ٲ��ָ� ��.
	chat_payload.header.type = S2C_CHAT;
	out_packet->write(&chat_payload, sizeof(cs_packet_chat));	//���Ⱑ �� �����ΰ�;
	out_packet->add_ref( local_view_list.size() );
	for (auto& it : local_view_list)
	{
		mmo_server->send_packet(it.second->session_id, out_packet);	// 
	}

	out_packet->decrease_ref_count();
}

