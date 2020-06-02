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

////////////////////////////////// 들어온 정보로 클라이언트 업데이트.
	cs_packet_login login_payload;
	in_packet.read(&login_payload, sizeof(cs_packet_login));
	
	mmo_actor->reset();
	memcpy(mmo_actor->name, login_payload.name, c2::constant::MAX_ID_LEN); // 어차피 여기서만 write 함. and 나갈 때;;
	if (mmo_session->get_actor()->get_id() != (uint16_t)session->session_id) // 이미 나갔다 들어온 녀석.
	{
		return;
	}

	mmo_session->response_loginok();		// 로그인 처리 및 응답
	mmo_zone->enter_actor(mmo_actor);
	



	MMOSector* current_sector = mmo_zone->get_sector(mmo_actor);			// view_list 긁어오기.
	const MMONear* nears = current_sector->get_near(mmo_actor->y, mmo_actor->x); // 주벽 섹터들.
	int near_cnt = nears->count;
	AcquireSRWLockExclusive(&mmo_actor->lock); //내 view_list 에 접근하기 쓰기 위해서 락을 얻고 
	for (int n = 0; n < near_cnt; ++n)
	{
		AcquireSRWLockShared(&nears->sectors[n]->lock); //sector에 읽기 위해서 락을 얻고 
		for (auto& other_iter : nears->sectors[n]->actors)
		{
			if (other_iter.second == mmo_actor)
				continue;

			if (mmo_actor->is_near(other_iter.second) == true) // 근처가 맞다면 넣음.
				mmo_actor->view_list.insert(other_iter);
		}

		 //NPC 처리 로직.
		for (auto other_iter : nears->sectors[n]->npcs) 
		{
			MMONpc* npc = mmo_npc_mgr.get_npc(other_iter);
			if (mmo_actor->is_near(npc) == true) // 근처가 맞다면 넣음.
			{
				mmo_actor->view_list_for_npc.insert(other_iter);			// 배 npc용 시야 리스트에 넣어줌.
			}
		}

		ReleaseSRWLockShared(&nears->sectors[n]->lock);
	}
	ReleaseSRWLockExclusive(&mmo_actor->lock);
	


////////// 내정보를 상대방에 보내고 나는 상대방 정보를 받는다. 
	c2::Packet* my_info_packet = c2::Packet::alloc();				// 주변에 보내기 위한 내정보 
	sc_packet_enter my_info_payload{ {sizeof(sc_packet_enter), S2C_ENTER}, (int16_t)mmo_session->session_id, {}, 0, mmo_actor->x , mmo_actor->y};
	memcpy(my_info_payload.name, mmo_actor->name, 50);				
	my_info_packet->write(&my_info_payload, sizeof(sc_packet_enter));



	sc_packet_enter other_info_payload;								// 고정된 타인 정보
	other_info_payload.header.length = sizeof(sc_packet_enter);
	other_info_payload.header.type = S2C_ENTER;



	AcquireSRWLockShared(&mmo_actor->lock);							//내 view_list 에 접근하기 읽기 위해서 락을 얻고 
	my_info_packet->add_ref( mmo_actor->view_list.size() );			// 락 밖에서 하면 언제  사이즈가 변경되어 있을지 모름.
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
		
		AcquireSRWLockExclusive(&other->lock); //내 view_list 에 접근하기 쓰기 위해서 락을 얻고 
		other->view_list.emplace(mmo_actor->get_id(), mmo_actor);
		ReleaseSRWLockExclusive(&other->lock); //내 view_list 에 접근하기 쓰기 위해서 락을 얻고 
		
		mmo_server->send_packet(mmo_actor->session_id, other_info_packet);			
		mmo_server->send_packet(other->session_id, my_info_packet);
	}

	for (auto npc_id : mmo_actor->view_list_for_npc)
	{
		MMONpc* npc = mmo_npc_mgr.get_npc(npc_id);
		AcquireSRWLockExclusive(&npc->lock); //내 view_list 에 접근하기 쓰기 위해서 락을 얻고 
		npc->view_list.emplace(mmo_actor->get_id(), mmo_actor);		// 서로 시야 리스트에 넣어줌.
		ReleaseSRWLockExclusive(&npc->lock); //내 view_list 에 접근하기 쓰기 위해서 락을 얻고 
		mmo_actor->wake_up_npc(npc);
	}


	ReleaseSRWLockShared(&mmo_actor->lock); 

	my_info_packet->decrease_ref_count();						// packet 릴리즈용 





	return;
}


#include "function.h"

REGISTER_HANDLER(C2S_MOVE)
{
	MMOSession* mmo_session = (MMOSession*)session;
	MMOActor*	my_actor	= mmo_session->get_actor();
	MMOServer*	mmo_server	= (MMOServer*)session->server;
	MMOZone*	mmo_zone	= mmo_server->get_zone();


	cs_packet_move cs_move_payload;								//	들어온 이동정보.
	in_packet.read(&cs_move_payload, sizeof(cs_packet_move));	// 


	// 사본 만들기.
	int local_y = my_actor->y;
	int local_x = my_actor->x;
	int local_actor_id = my_actor->get_id();

	
	// 장애물 체크 등등.
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
	MMOSector* prev_sector = my_actor->current_sector;					// view_list 긁어오기.
	MMOSector* curent_sector = mmo_zone->get_sector(local_y, local_x);			// view_list 긁어오기.


	if (prev_sector != curent_sector)										//섹터가 바뀐 경우.
	{
		AcquireSRWLockExclusive(&curent_sector->lock);						// 이전 섹터에서 나가기 위해서.
		prev_sector->actors.erase(local_actor_id);
		ReleaseSRWLockExclusive(&curent_sector->lock);						

		AcquireSRWLockExclusive(&curent_sector->lock);						// 내 view_list 에 접근하기 쓰기 위해서 락을 얻고 
		curent_sector->actors.emplace(local_actor_id, my_actor);
		my_actor->current_sector = curent_sector;							// 타 스레드에서 접근하면 여기 일로 하고?
		ReleaseSRWLockExclusive(&curent_sector->lock);						// 내 view_list 에 접근하기 쓰기 위해서 락을 얻고 
	}






	AcquireSRWLockShared(&my_actor->lock);
	std::map<int32_t, MMOActor*> local_old_view_list = my_actor->view_list;
	ReleaseSRWLockShared(&my_actor->lock);



	/// 섹터 구하기...
	MMOSector*		current_sector	= curent_sector;
	const MMONear*	nears			= current_sector->get_near(local_y, local_x);
	int				near_cnt		= nears->count;



	// 내 주변 정보를 긁어 모음.
	std::map<int32_t, MMOActor*>	local_new_view_list;
	for (int n = 0; n < near_cnt; ++n)					
	{
		AcquireSRWLockShared(&nears->sectors[n]->lock); // sector에 읽기 위해서 락을 얻고 
		for (auto& actor_iter : nears->sectors[n]->actors)
		{
			//if (actor_iter.second->status != ST_ACTIVE) 
				//continue;
			if (actor_iter.second == my_actor)
				continue;

			if (my_actor->is_near(actor_iter.second) == true) // 내 근처가 맞다면 넣음.
				local_new_view_list.insert(actor_iter);
		}
		ReleaseSRWLockShared(&nears->sectors[n]->lock);
	}
	
	
	
	c2::Packet*			my_move_packet = c2::Packet::alloc();
	sc_packet_move		sc_move_payload{ {sizeof(sc_packet_move), S2C_MOVE}, local_actor_id, local_x, local_y, cs_move_payload.move_time };

	my_move_packet->write(&sc_move_payload, sizeof(sc_packet_move));  // 나한테 내 이동전송.
	mmo_server->send_packet(my_actor->session_id, my_move_packet);


///////////////////
	for (auto& new_actor : local_new_view_list)
	{
		if( 0 == local_old_view_list.count(new_actor.first) ) // 이동후 새로 보이는 유저.
		{
			my_actor->send_enter_packet(new_actor.second);		// 타인 정보를 나한테 보냄.

			AcquireSRWLockShared(&new_actor.second->lock);							
			if ( 0 == new_actor.second->view_list.count(my_actor->get_id()) ) // 타 스레드에서 시야 처리 안 된경우.
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_enter_packet(my_actor);
			}
			else // 처리 된경우
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_move_packet(my_actor);   // 상대 시야 리스트에 내가 있는 경우 뷰리스트만 업데이트 한다.
			}
		}
		else  // 기존 뷰리스트에 있던 유저들 
		{
			AcquireSRWLockShared(&new_actor.second->lock);					//
			if (0 != new_actor.second->view_list.count(my_actor->get_id()))	// 
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_move_packet(my_actor);
			}
			else	// 이미 나간 경우.
			{
				ReleaseSRWLockShared(&new_actor.second->lock);

				new_actor.second->send_enter_packet(my_actor);
			}
		}
	}

	//시야에서 벗어난 플레이어
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
			else	// 이미 다른 스레드에서 지워준 경우.
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

	//		out_packet->write(&chat_payload, sizeof(cs_packet_chat));	//여기가 좀 병목인가;
	//		mmo_server->send_packet(neighbor->session_id, out_packet);	// 
	//	}

	//	ReleaseSRWLockShared(&n_sector->lock);
	//}
}

