#include "../C2Server/C2Server/packet_handler.h"
#include "../C2Server/C2Server/OuterServer.h"
#include "MMOSession.h"
#include "MMOServer.h"
#include "MMOZone.h"


REGISTER_HANDLER(C2S_LOGIN)
{
	MMOSession* mmo_session		= (MMOSession*)session;
	MMOActor*	mmo_actor		= mmo_session->get_actor();
	MMOServer*	mmo_server		= (MMOServer*)session->server;
	mmo_actor->zone = mmo_server->get_zone();
	mmo_actor->server = mmo_server;
	MMOZone*	mmo_zone		= (MMOZone*)mmo_actor->zone;


////////////////////////////////// ���� ������ Ŭ���̾�Ʈ ������Ʈ.
	cs_packet_login login_payload;
	in_packet.read(&login_payload, sizeof(cs_packet_login));
	mmo_actor->reset();
	mmo_actor->status = ST_ACTIVE;
	memcpy(mmo_actor->name, login_payload.name, c2::constant::MAX_ID_LEN);
	if (mmo_session->get_actor()->get_id() != (uint16_t)session->session_id) // �̹� ������ ���� �༮.
	{
		return;
	}



////////////////////////////////////// ���� ����� Ŭ�󿡰� �˸�.
	sc_packet_login_ok loginok_payload; 									// �ֺ��� ������ ���� ������ // ���߿� DB ��û���� ����.... break;
	mmo_actor->get_login_packet_info(loginok_payload);						// dbó���� �񵿱�� �ٲ�� ��;
	c2::Packet* loginok_packet = c2::Packet::alloc();						//
	loginok_packet->write(&loginok_payload, sizeof(sc_packet_login_ok));	//
	mmo_server->send_packet(mmo_actor->session_id, loginok_packet);			// Ŭ�󿡰� ���� �˷��� ������

																			//////////////////////// view_list �ܾ����.
	MMOSector* current_sector = mmo_zone->get_sector(mmo_actor->y, mmo_actor->x);
	AcquireSRWLockExclusive(&current_sector->lock); //�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
	current_sector->actors.emplace(mmo_actor->get_id(), mmo_actor );
	ReleaseSRWLockExclusive(&current_sector->lock); //�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 

	//InterlockedExchangePointer((void* volatile*)mmo_actor->current_sector, current_sector);

	const MMONear* nears = current_sector->get_near(mmo_actor->y, mmo_actor->x); // �ֺ� ���͵�.


	int near_cnt = nears->count;
	AcquireSRWLockExclusive(&mmo_actor->lock); //�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
	for (int n = 0; n < near_cnt; ++n)
	{
		AcquireSRWLockShared(&nears->sectors[n]->lock); //sector�� �б� ���ؼ� ���� ��� 

		for (auto& actor_iter : nears->sectors[n]->actors)
		{
			if (actor_iter.second == mmo_actor)
				continue;

			if (mmo_actor->is_near(actor_iter.second) == true) // ��ó�� �´ٸ� ����.
				mmo_actor->view_list.insert(actor_iter);
		}

		ReleaseSRWLockShared(&nears->sectors[n]->lock);
	}
	ReleaseSRWLockExclusive(&mmo_actor->lock);
	


////////// �������� ���濡 ������ ���� ���� ������ �޴´�. 
	c2::Packet* myinfo_packet = c2::Packet::alloc();				// �ֺ��� ������ ���� ������ 
	sc_packet_enter my_info_payload;								
	my_info_payload.header.length = sizeof(sc_packet_enter);
	my_info_payload.header.type = S2C_ENTER;
	my_info_payload.id = (int16_t)mmo_session->session_id;
	memcpy(my_info_payload.name, mmo_actor->name, 50);				//my_info_payload.o_type;
	my_info_payload.x = mmo_actor->x;
	my_info_payload.y = mmo_actor->y;
	myinfo_packet->write(&my_info_payload, sizeof(sc_packet_enter));

	sc_packet_enter other_info_payload;								// ������ Ÿ�� ����
	other_info_payload.header.length = sizeof(sc_packet_enter);
	other_info_payload.header.type = S2C_ENTER;

	AcquireSRWLockShared(&mmo_actor->lock);							//�� view_list �� �����ϱ� �б� ���ؼ� ���� ��� 
	myinfo_packet->add_ref( mmo_actor->view_list.size() );			// �� �ۿ��� �ϸ� ����  ����� ����Ǿ� ������ ��.
	for ( auto& iter : mmo_actor->view_list )
	{
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
		mmo_server->send_packet(other->session_id, myinfo_packet);
	}
	ReleaseSRWLockShared(&mmo_actor->lock); 

	myinfo_packet->decrease_ref_count();						// packet ������� 

	return;
}
#include "function.h"

REGISTER_HANDLER(C2S_MOVE)
{
	MMOSession* mmo_session = (MMOSession*)session;
	MMOActor*	mmo_actor	= mmo_session->get_actor();
	MMOServer*	mmo_server	= (MMOServer*)session->server;
	MMOZone*	mmo_zone	= mmo_server->get_zone();

	cs_packet_move cs_move_payload;							//	���� �̵�����.
	in_packet.read(&cs_move_payload, sizeof(cs_packet_move));	// 


	int y = mmo_actor->y, x = mmo_actor->x;
	// ��ֹ� üũ ���.
	switch (cs_move_payload.direction)
	{
	case c2::constant::D_DOWN:
		y = clamp(0, y + 1, 399);
		mmo_actor->direction = NEAR_DOWN;
		break;
	case c2::constant::D_LEFT:
		x = clamp(0, x - 1, 399);
		mmo_actor->direction = NEAR_LEFT;
		break;
	case c2::constant::D_RIGHT:
		x = clamp(0, x + 1, 399);
		mmo_actor->direction = NEAR_RIGHT;
		break;
	case c2::constant::D_UP:
		y = clamp(0, y - 1, 399);
		mmo_actor->direction = NEAR_UP;
		break;
	}

	mmo_actor->x = x;
	mmo_actor->y = y;
	

	AcquireSRWLockShared(&mmo_actor->lock);
	std::map<int32_t, MMOActor*> old_view_list = mmo_actor->view_list;
	ReleaseSRWLockShared(&mmo_actor->lock);

	/// ���� ���ϱ�...
	MMOSector* current_sector = mmo_zone->get_sector(y, x);
	const MMONear* nears = current_sector->get_near(y, x);
	int near_cnt = nears->count;

	std::map<int32_t, MMOActor*> new_view_list;

	for (int n = 0; n < near_cnt; ++n)
	{
		AcquireSRWLockShared(&nears->sectors[n]->lock); //sector�� �б� ���ؼ� ���� ��� 
		for (auto& actor_iter : nears->sectors[n]->actors)
		{
			//if (actor_iter.second->status != ST_ACTIVE) 
				//continue;
			if (actor_iter.second == mmo_actor)
				continue;
			if (mmo_actor->is_near(actor_iter.second) == true) // ��ó�� �´ٸ� ����.
				new_view_list.insert(actor_iter);
		}
		ReleaseSRWLockShared(&nears->sectors[n]->lock);
	}
	

	c2::Packet* my_move_packet = c2::Packet::alloc();

	sc_packet_move sc_move_payload;
	sc_move_payload.header.length = sizeof(sc_packet_move);
	sc_move_payload.header.type = S2C_MOVE;
	sc_move_payload.x = x;
	sc_move_payload.y = y;
	sc_move_payload.id = mmo_actor->get_id();
	sc_move_payload.move_time = cs_move_payload.move_time;
	my_move_packet->write(&sc_move_payload, sizeof(sc_packet_move));  // ������ �� �̵�����.
	mmo_server->send_packet(mmo_actor->session_id, my_move_packet);

	for (auto& new_it : new_view_list)
	{
		if( 0 == old_view_list.count(new_it.first) ) // �̵��� ���� ���̴� ����.
		{
			new_it.second->send_enter_packet(mmo_actor);		// Ÿ�� ������ ������ ����.

			AcquireSRWLockShared(&new_it.second->lock);							
			if ( 0 == new_it.second->view_list.count(mmo_actor->get_id()) ) // Ÿ �����忡�� �þ� ó�� �� �Ȱ��.
			{
				ReleaseSRWLockShared(&new_it.second->lock);

				mmo_actor->send_enter_packet(new_it.second);
			}
			else // ó�� �Ȱ��
			{
				ReleaseSRWLockShared(&new_it.second->lock);

				mmo_actor->send_move_packet(new_it.second);
			}
		}
		else  // ���� �丮��Ʈ�� �ִ� ������ 
		{
			AcquireSRWLockShared(&new_it.second->lock);							//
			if (0 != new_it.second->view_list.count(mmo_actor->get_id()))	// 
			{
				ReleaseSRWLockShared(&new_it.second->lock);

				mmo_actor->send_move_packet(new_it.second);
			}
			else	// �̹� ���� ���.
			{
				ReleaseSRWLockShared(&new_it.second->lock);

				mmo_actor->send_enter_packet(new_it.second);
			}
		}
	}

	//�þ߿��� ��� �÷��̾�
	for (auto& old_it : old_view_list)
	{
		if (0 == new_view_list.count(old_it.first))
		{
			old_it.second->send_leave_packet(mmo_actor);

			AcquireSRWLockShared(&old_it.second->lock);
			if (0 != old_it.second->view_list.count(mmo_actor->get_id()))
			{
				ReleaseSRWLockShared(&old_it.second->lock);
				mmo_actor->send_leave_packet(old_it.second);
			}
			else
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

