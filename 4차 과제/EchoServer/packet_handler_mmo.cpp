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


	// 들어와서 
	// 내 위치 기준으로 주변을 찾고 
	// Sector read 락을 걸고 주변에 포함 되는 애들을 얻는다. // 이중 락킹.
	// 내 뷰리스트 락을 걸로 내 뷰리스트 업데이트            // 이중 락킹.
	// 상대 애들 정보를 가져옴.
	if (mmo_session->get_actor()->get_id() != (uint16_t)session->session_id) // 이미 나갔다 들어온 녀석.
	{
		return;
	}

	mmo_actor->reset();
	memcpy(mmo_actor->name, login_payload.name, c2::constant::MAX_ID_LEN); 
	

	sc_packet_login_ok loginok_payload; 								// 주변에 보내기 위한 내정보 // 나중엔 DB 요청으로 변경.... break;
	mmo_actor->get_login_packet_info(loginok_payload);					// db처리로 비동기로 바꿔야 함;
	c2::Packet* out_packet = c2::Packet::alloc();						//
	out_packet->write(&loginok_payload, sizeof(sc_packet_login_ok));	//
	mmo_server->send_packet(mmo_actor->session_id, out_packet);			// 클라에게 통지 요청.

	
	MMOSector* enter_sector = mmo_zone->get_sector(mmo_actor->y, mmo_actor->x);

	sc_packet_enter my_info_payload;							// 주변에 보내기 위한 내정보 
	my_info_payload.header.length = sizeof(sc_packet_enter);
	my_info_payload.header.type = S2C_ENTER; 
	my_info_payload.id = (int16_t)mmo_session->session_id;
	memcpy(my_info_payload.name, mmo_actor->name, 50);				//my_info_payload.o_type; // 아마 오브젝트 타입인듯.
	my_info_payload.x = mmo_actor->x; 
	my_info_payload.y = mmo_actor->y;


	auto near_sectors = mmo_actor->current_sector->near_sectors;//  주변 섹터 구해서 
	for (MMOSector* sector : near_sectors)
	{
		AcquireSRWLockShared(&sector->lock); //sector에 접근하기 위해서 락을 얻고 
		auto actors = sector->actors;

		for (auto& it : actors) // 주변 인원들에게 내 정보 페킷 만들어서 발사.
		{
			c2::Packet* myinfo_packet = c2::Packet::alloc();  
			c2::Packet* other_info_packet = c2::Packet::alloc();
			

			MMOActor* other = it.second;
			sc_packet_enter other_info_payload;// 타인 정보
			other_info_payload.header.length = sizeof(sc_packet_enter);
			other_info_payload.header.type = S2C_ENTER;
			other_info_payload.id = other->get_id();
			memcpy(other_info_payload.name, other->name, sizeof(sc_packet_enter::name));
			other_info_payload.x = other->x;
			other_info_payload.y = other->y;

			
			myinfo_packet->write(&my_info_payload, sizeof(sc_packet_enter)); // 내정보 타인한테 보내기
			other_info_packet->write(&other_info_payload, sizeof(sc_packet_enter));// 타인정보 나한테 보내기

			mmo_session->server->send_packet(other->session_id, myinfo_packet); // 내정보 남한테 보내기
			mmo_session->server->send_packet(mmo_actor->session_id, other_info_packet); // 타인정보 나한테 보내기
		}

		ReleaseSRWLockShared(&sector->lock); // 섹터에 벗어나니 락을 해제한다.
	}
	
	
	
	AcquireSRWLockExclusive(&mmo_zone->lock);
	mmo_zone->actors.emplace(mmo_actor); // zone에서 받고.
	ReleaseSRWLockExclusive(&mmo_zone->lock);

	return;
}

REGISTER_HANDLER(C2S_MOVE)
{
	MMOSession* mmo_session = (MMOSession*)session;
	MMOActor*	mmo_actor	= mmo_session->get_actor();
	MMOServer*	mmo_server	= (MMOServer*)session->server;
	MMOZone*	mmo_zone	= (MMOZone*)mmo_actor->zone;


	cs_packet_move move_payload;							//	들어온 이동정보.
	in_packet.read(&move_payload, sizeof(cs_packet_move));	// 


	mmo_actor->move(move_payload.direction);				// 자신을 건들떄 락을 써야하나? 
	MMOSector* current_sector = mmo_actor->current_sector;
	MMOSector* new_sector = mmo_zone->get_sector(mmo_actor->y, mmo_actor->x);

	// 나는 나가고 
	// 상대방 애드에게 나간다.

	// 들어간다.

	// 상대방 정보 가져온다.
	// 겹치는 애들 이동한다.

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
	


	sc_packet_leave my_leave_payload;								// 내가 나가는 영역에 대한 처리.
	my_leave_payload.header.length = sizeof(sc_packet_leave);		
	my_leave_payload.header.type = S2C_LEAVE;
	my_leave_payload.id = mmo_actor->get_id();


	sc_packet_enter my_enter_payload;								// 내가 들어가는 영역에 내정보 타인한테 보내기
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

			mmo_server->send_packet(other->session_id, my_leave_packet); // 타인정보 나한테 보내기
			mmo_server->send_packet(mmo_actor->session_id, other_leave_packet); // 타인정보 나한테 보내기
		}



		sc_packet_move move_payload;
		move_payload.header.length = sizeof(sc_packet_move);
		move_payload.header.type = S2C_MOVE;
		move_payload.x = mmo_actor->x;
		move_payload.y = mmo_actor->y;
		move_payload.id = mmo_actor->get_id();
		move_payload.move_time = mmo_actor->last_move_time;


		
		for (MMOSector* inter_sector : *intersection_sectors) // move 보낼 곳.
		{
			std::map<uint16_t, MMOActor*>& inter_actors = inter_sector->actors;

			for (auto& it : inter_actors)
			{
				c2::Packet* out_packet = c2::Packet::alloc();
				out_packet->write(&move_payload, sizeof(sc_packet_move));
				mmo_server->send_packet(it.second->session_id, out_packet); // 타인정보 나한테 보내기
			}

		}



		// enter 보낼 곳.
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

				mmo_server->send_packet(other->session_id, my_enter_packet); // 타인정보 나한테 보내기
				mmo_server->send_packet(mmo_actor->session_id, other_enter_packet); // 타인정보 나한테 보내기
			}
		}

		/// 마지막으로 sector 반영.
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

			out_packet->write(&chat_payload, sizeof(cs_packet_chat));	//여기가 좀 병목인가;
			mmo_server->send_packet(neighbor->session_id, out_packet);	// 
		}

		ReleaseSRWLockShared(&n_sector->lock);
	}
}

