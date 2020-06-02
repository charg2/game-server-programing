#include "../C2Server/C2Server/packet_handler.h"
#include "../C2Server/C2Server/OuterServer.h"
#include "MMOSession.h"
#include "MMOServer.h"
#include "MMOZone.h"
#include "MMONpcManager.h"


REGISTER_HANDLER(C2S_LOGIN)
{
	MMOSession* mmo_session		= (MMOSession*)session;
	MMOActor*	mmo_actor		= mmo_session->get_actor();
	MMOServer*	mmo_server		= (MMOServer*)session->server;
	mmo_actor->zone				= mmo_server->get_zone();
	mmo_actor->server			= mmo_server;
	MMOZone*	mmo_zone		= (MMOZone*)mmo_actor->zone;
	MMONpcManager mmo_npc_mgr	= MMONpcManager::instance();

////////////////////////////////// ���� ������ Ŭ���̾�Ʈ ������Ʈ.
	cs_packet_login login_payload;
	in_packet.read(&login_payload, sizeof(cs_packet_login));
	
	mmo_actor->reset();
	memcpy(mmo_actor->name, login_payload.name, c2::constant::MAX_ID_LEN); // ������ ���⼭�� write ��. and ���� ��;;
	if (mmo_session->get_actor()->get_id() != (uint16_t)session->session_id) // �̹� ������ ���� �༮.
	{
		return;
	}

	mmo_session->response_loginok();		// �α��� ó�� �� ����
	mmo_zone->enter_actor(mmo_actor);
	



	MMOSector* current_sector = mmo_zone->get_sector(mmo_actor);			// view_list �ܾ����.
	const MMONear* nears = current_sector->get_near(mmo_actor->y, mmo_actor->x); // �ֺ� ���͵�.
	int near_cnt = nears->count;
	AcquireSRWLockExclusive(&mmo_actor->lock); //�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
	for (int n = 0; n < near_cnt; ++n)
	{
		AcquireSRWLockShared(&nears->sectors[n]->lock); //sector�� �б� ���ؼ� ���� ��� 
		for (auto& other_iter : nears->sectors[n]->actors)
		{
			if (other_iter.second == mmo_actor)
				continue;

			if (mmo_actor->is_near(other_iter.second) == true) // ��ó�� �´ٸ� ����.
				mmo_actor->view_list.insert(other_iter);
		}

		 //NPC ó�� ����.
		for (auto other_iter : nears->sectors[n]->npcs) 
		{
			MMONpc* npc = mmo_npc_mgr.get_npc(other_iter);
			if (mmo_actor->is_near(npc) == true) // ��ó�� �´ٸ� ����.
			{
				mmo_actor->view_list_for_npc.insert(other_iter);			// �� npc�� �þ� ����Ʈ�� �־���.
			}
		}

		ReleaseSRWLockShared(&nears->sectors[n]->lock);
	}
	ReleaseSRWLockExclusive(&mmo_actor->lock);
	


////////// �������� ���濡 ������ ���� ���� ������ �޴´�. 
	c2::Packet* my_info_packet = c2::Packet::alloc();				// �ֺ��� ������ ���� ������ 
	sc_packet_enter my_info_payload{ {sizeof(sc_packet_enter), S2C_ENTER}, (int16_t)mmo_session->session_id, {}, 0, mmo_actor->x , mmo_actor->y};
	memcpy(my_info_payload.name, mmo_actor->name, 50);				
	my_info_packet->write(&my_info_payload, sizeof(sc_packet_enter));



	sc_packet_enter other_info_payload;								// ������ Ÿ�� ����
	other_info_payload.header.length = sizeof(sc_packet_enter);
	other_info_payload.header.type = S2C_ENTER;



	AcquireSRWLockShared(&mmo_actor->lock);							//�� view_list �� �����ϱ� �б� ���ؼ� ���� ��� 
	my_info_packet->add_ref( mmo_actor->view_list.size() );			// �� �ۿ��� �ϸ� ����  ����� ����Ǿ� ������ ��.
	for ( auto& iter : mmo_actor->view_list )
	{
		if (iter.second == mmo_actor)
			continue;
		
		MMOActor* other = iter.second;
		
		c2::Packet* other_info_packet = c2::Packet::alloc();						//
		other_info_payload.id = other->get_id();
		memcpy(other_info_payload.name, other->name, sizeof(sc_packet_enter::name));
		other_info_payload.x = other->x;
		other_info_payload.y = other->y;
		other_info_packet->write(&other_info_payload, sizeof(other_info_payload));
		
		AcquireSRWLockExclusive(&other->lock); //�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
		other->view_list.emplace(mmo_actor->get_id(), mmo_actor);
		ReleaseSRWLockExclusive(&other->lock); //�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
		
		mmo_server->send_packet(mmo_actor->session_id, other_info_packet);			
		mmo_server->send_packet(other->session_id, my_info_packet);
	}

	for (auto npc_id : mmo_actor->view_list_for_npc)
	{
		MMONpc* npc = mmo_npc_mgr.get_npc(npc_id);
		AcquireSRWLockExclusive(&npc->lock); //�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
		npc->view_list.emplace(mmo_actor->get_id(), mmo_actor);		// ���� �þ� ����Ʈ�� �־���.
		ReleaseSRWLockExclusive(&npc->lock); //�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
		mmo_actor->wake_up_npc(npc);
	}


	ReleaseSRWLockShared(&mmo_actor->lock); 

	my_info_packet->decrease_ref_count();						// packet ������� 





	return;
}


#include "function.h"

REGISTER_HANDLER(C2S_MOVE)
{
	MMOSession* mmo_session = (MMOSession*)session;
	MMOActor*	my_actor	= mmo_session->get_actor();
	MMOServer*	mmo_server	= (MMOServer*)session->server;
	MMOZone*	mmo_zone	= mmo_server->get_zone();


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
	std::map<int32_t, MMOActor*> local_old_view_list = my_actor->view_list;
	ReleaseSRWLockShared(&my_actor->lock);



	/// ���� ���ϱ�...
	MMOSector*		current_sector	= curent_sector;
	const MMONear*	nears			= current_sector->get_near(local_y, local_x);
	int				near_cnt		= nears->count;



	// �� �ֺ� ������ �ܾ� ����.
	std::map<int32_t, MMOActor*>	local_new_view_list;
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
}

REGISTER_HANDLER(C2S_CHAT)
{
	//MMOSession* mmo_session		{ (MMOSession*)session };
	//MMOActor*	mmo_actor		{ mmo_session->get_actor() };
	//MMOServer*	mmo_server		{ (MMOServer*)session->server };
	//MMOSector*	sector			{ mmo_actor->current_sector };


	//cs_packet_chat chat_payload; 								// id check
	//in_packet.read(&chat_payload, sizeof(cs_packet_chat));

	//std::vector<MMOSector*>& near_sectors = sector->near_sectors;
	//for ( MMOSector* n_sector : near_sectors)
	//{
	//	AcquireSRWLockShared(&n_sector->lock);
	//	auto& actors = n_sector->actors;

	//	for (auto& iter : actors)
	//	{
	//		MMOActor*	neighbor	= iter.second;
	//		c2::Packet* out_packet	=  c2::Packet::alloc();

	//		out_packet->write(&chat_payload, sizeof(cs_packet_chat));	//���Ⱑ �� �����ΰ�;
	//		mmo_server->send_packet(neighbor->session_id, out_packet);	// 
	//	}

	//	ReleaseSRWLockShared(&n_sector->lock);
	//}
}

