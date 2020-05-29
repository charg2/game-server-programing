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
	MMOZone*	mmo_zone		= (MMOZone*)mmo_actor->zone;

	cs_packet_login login_payload;
	in_packet.read(&login_payload, sizeof(cs_packet_login));


	// ���ͼ� 
	// �� ��ġ �������� �ֺ��� ã�� 
	// Sector read ���� �ɰ� �ֺ��� ���� �Ǵ� �ֵ��� ��´�. // ���� ��ŷ.
	// �� �丮��Ʈ ���� �ɷ� �� �丮��Ʈ ������Ʈ            // ���� ��ŷ.
	// ��� �ֵ� ������ ������.
	if (mmo_session->get_actor()->get_id() != (uint16_t)session->session_id) // �̹� ������ ���� �༮.
	{
		return;
	}

	mmo_actor->reset();
	memcpy(mmo_actor->name, login_payload.name, c2::constant::MAX_ID_LEN); 
	

	sc_packet_login_ok loginok_payload; 								// �ֺ��� ������ ���� ������ // ���߿� DB ��û���� ����.... break;
	mmo_actor->get_login_packet_info(loginok_payload);					// dbó���� �񵿱�� �ٲ�� ��;
	c2::Packet* out_packet = c2::Packet::alloc();						//
	out_packet->write(&loginok_payload, sizeof(sc_packet_login_ok));	//
	mmo_server->send_packet(mmo_actor->session_id, out_packet);			// Ŭ�󿡰� ���� ��û.

	
	MMOSector* enter_sector = mmo_zone->get_sector(mmo_actor->y, mmo_actor->x);

	sc_packet_enter my_info_payload;							// �ֺ��� ������ ���� ������ 
	my_info_payload.header.length = sizeof(sc_packet_enter);
	my_info_payload.header.type = S2C_ENTER; 
	my_info_payload.id = (int16_t)mmo_session->session_id;
	memcpy(my_info_payload.name, mmo_actor->name, 50);				//my_info_payload.o_type; // �Ƹ� ������Ʈ Ÿ���ε�.
	my_info_payload.x = mmo_actor->x; 
	my_info_payload.y = mmo_actor->y;


	auto near_sectors = mmo_actor->current_sector->near_sectors;//  �ֺ� ���� ���ؼ� 
	for (MMOSector* sector : near_sectors)
	{
		AcquireSRWLockShared(&sector->lock); //sector�� �����ϱ� ���ؼ� ���� ��� 
		auto actors = sector->actors;

		for (auto& it : actors) // �ֺ� �ο��鿡�� �� ���� ��Ŷ ���� �߻�.
		{
			c2::Packet* myinfo_packet = c2::Packet::alloc();  
			c2::Packet* other_info_packet = c2::Packet::alloc();
			

			MMOActor* other = it.second;
			sc_packet_enter other_info_payload;// Ÿ�� ����
			other_info_payload.header.length = sizeof(sc_packet_enter);
			other_info_payload.header.type = S2C_ENTER;
			other_info_payload.id = other->get_id();
			memcpy(other_info_payload.name, other->name, sizeof(sc_packet_enter::name));
			other_info_payload.x = other->x;
			other_info_payload.y = other->y;

			
			myinfo_packet->write(&my_info_payload, sizeof(sc_packet_enter)); // ������ Ÿ������ ������
			other_info_packet->write(&other_info_payload, sizeof(sc_packet_enter));// Ÿ������ ������ ������

			mmo_session->server->send_packet(other->session_id, myinfo_packet); // ������ ������ ������
			mmo_session->server->send_packet(mmo_actor->session_id, other_info_packet); // Ÿ������ ������ ������
		}

		ReleaseSRWLockShared(&sector->lock); // ���Ϳ� ����� ���� �����Ѵ�.
	}
	
	
	
	AcquireSRWLockExclusive(&mmo_zone->lock);
	mmo_zone->actors.emplace(mmo_actor); // zone���� �ް�.
	ReleaseSRWLockExclusive(&mmo_zone->lock);

	return;
}

REGISTER_HANDLER(C2S_MOVE)
{
	MMOSession* mmo_session = (MMOSession*)session;
	MMOActor*	mmo_actor	= mmo_session->get_actor();
	MMOServer*	mmo_server	= (MMOServer*)session->server;
	MMOZone*	mmo_zone	= (MMOZone*)mmo_actor->zone;


	cs_packet_move move_payload;							//	���� �̵�����.
	in_packet.read(&move_payload, sizeof(cs_packet_move));	// 


	mmo_actor->move(move_payload.direction);				// �ڽ��� �ǵ鋚 ���� ����ϳ�? 
	MMOSector* current_sector = mmo_actor->current_sector;
	MMOSector* new_sector = mmo_zone->get_sector(mmo_actor->y, mmo_actor->x);

	// ���� ������ 
	// ���� �ֵ忡�� ������.

	// ����.

	// ���� ���� �����´�.
	// ��ġ�� �ֵ� �̵��Ѵ�.

	if ( current_sector != new_sector )
	{
		AcquireSRWLockExclusive(&current_sector->lock);
		current_sector->actors.erase(mmo_actor->get_id());
		for (auto& it : current_sector->actors)
		{
			it.second;


		}
		ReleaseSRWLockExclusive(&current_sector->lock);
		
		AcquireSRWLockExclusive(&new_sector->lock);
		new_sector->actors.emplace(mmo_actor->get_id(), mmo_actor);
		ReleaseSRWLockExclusive(&new_sector->lock);
	}
	


	sc_packet_leave my_leave_payload;								// ���� ������ ������ ���� ó��.
	my_leave_payload.header.length = sizeof(sc_packet_leave);		
	my_leave_payload.header.type = S2C_LEAVE;
	my_leave_payload.id = mmo_actor->get_id();


	sc_packet_enter my_enter_payload;								// ���� ���� ������ ������ Ÿ������ ������
	my_enter_payload.header.length = sizeof(sc_packet_enter);
	my_enter_payload.header.type = S2C_ENTER;
	my_enter_payload.id = mmo_actor->get_id();
	memcpy(my_enter_payload.name, mmo_actor->name, sizeof(sc_packet_enter::name));
	my_enter_payload.x = mmo_actor->x;
	my_enter_payload.y = mmo_actor->y;



	auto& near_sectors = mmo_actor->current_sector->near_sectors;
	for (MMOSector* sector : near_sectors)
	{
		sector->lock;

		std::map<uint16_t, MMOActor*>& old_actors = sector->actors;

		for (auto& it : old_actors)
		{
			MMOActor* other = it.second;

			sc_packet_leave other_leave_payload;
			other_leave_payload.header.length = sizeof(sc_packet_leave);
			other_leave_payload.header.type = S2C_LEAVE;
			other_leave_payload.id = other->get_id();

			c2::Packet* my_leave_packet = c2::Packet::alloc();
			c2::Packet* other_leave_packet = c2::Packet::alloc();

			my_leave_packet->write(&my_leave_payload, sizeof(sc_packet_leave));
			other_leave_packet->write(&other_leave_payload, sizeof(sc_packet_leave));

			mmo_server->send_packet(other->session_id, my_leave_packet); // Ÿ������ ������ ������
			mmo_server->send_packet(mmo_actor->session_id, other_leave_packet); // Ÿ������ ������ ������
		}



		sc_packet_move move_payload;
		move_payload.header.length = sizeof(sc_packet_move);
		move_payload.header.type = S2C_MOVE;
		move_payload.x = mmo_actor->x;
		move_payload.y = mmo_actor->y;
		move_payload.id = mmo_actor->get_id();
		move_payload.move_time = mmo_actor->last_move_time;


		
		for (MMOSector* inter_sector : *intersection_sectors) // move ���� ��.
		{
			std::map<uint16_t, MMOActor*>& inter_actors = inter_sector->actors;

			for (auto& it : inter_actors)
			{
				c2::Packet* out_packet = c2::Packet::alloc();
				out_packet->write(&move_payload, sizeof(sc_packet_move));
				mmo_server->send_packet(it.second->session_id, out_packet); // Ÿ������ ������ ������
			}

		}



		// enter ���� ��.
		for (MMOSector* new_sector : *new_difference_sectors)
		{
			std::map<uint16_t, MMOActor*>& new_actors = new_sector->actors;

			for (auto& it : new_actors)
			{
				MMOActor* other = it.second;
				sc_packet_enter other_enter_payload;
				other_enter_payload.header.length = sizeof(sc_packet_enter);
				other_enter_payload.header.type = S2C_ENTER;
				other_enter_payload.id = other->get_id();
				memcpy(other_enter_payload.name, other->name, sizeof(sc_packet_enter::name));
				other_enter_payload.x = other->x;
				other_enter_payload.y = other->y;


				c2::Packet* my_enter_packet = c2::Packet::alloc();
				c2::Packet* other_enter_packet = c2::Packet::alloc();
				my_enter_packet->write(&my_enter_payload, sizeof(sc_packet_enter));
				other_enter_packet->write(&other_enter_payload, sizeof(sc_packet_enter));

				mmo_server->send_packet(other->session_id, my_enter_packet); // Ÿ������ ������ ������
				mmo_server->send_packet(mmo_actor->session_id, other_enter_packet); // Ÿ������ ������ ������
			}
		}

		/// ���������� sector �ݿ�.
		mmo_actor->last_move_time = 0;
		return;
	}
}

REGISTER_HANDLER(C2S_CHAT)
{
	MMOSession* mmo_session		{ (MMOSession*)session };
	MMOActor*	mmo_actor		{ mmo_session->get_actor() };
	MMOServer*	mmo_server		{ (MMOServer*)session->server };
	MMOSector*	sector			{ mmo_actor->current_sector };


	cs_packet_chat chat_payload; 								// id check
	in_packet.read(&chat_payload, sizeof(cs_packet_chat));

	std::vector<MMOSector*>& near_sectors = sector->near_sectors;
	for ( MMOSector* n_sector : near_sectors)
	{
		AcquireSRWLockShared(&n_sector->lock);
		auto& actors = n_sector->actors;

		for (auto& iter : actors)
		{
			MMOActor*	neighbor	= iter.second;
			c2::Packet* out_packet	=  c2::Packet::alloc();

			out_packet->write(&chat_payload, sizeof(cs_packet_chat));	//���Ⱑ �� �����ΰ�;
			mmo_server->send_packet(neighbor->session_id, out_packet);	// 
		}

		ReleaseSRWLockShared(&n_sector->lock);
	}
}

