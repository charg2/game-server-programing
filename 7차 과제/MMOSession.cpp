#include "protocol.h"
#include "MMOSession.h"
#include "MMOServer.h"

#include "DBHelper.h"
#include "MMODBTask.h"
#include "DBManager.h"
#include "MMOZone.h"
#include "MMONpcManager.h"

MMOSession::MMOSession() : actor{ this }
{}

MMOSession::~MMOSession()
{}

MMOActor* MMOSession::get_actor()
{
	return &actor;
}



void MMOSession::response_loginok()
{
	sc_packet_login_ok loginok_payload;  // �ֺ��� ������ ���� ������ // ���߿� DB ��û���� ����.... break;
	
	actor.get_login_packet_info(loginok_payload);						// dbó���� �񵿱�� �ٲ�� ��;

	c2::Packet* loginok_packet = c2::Packet::alloc();						

	loginok_packet->write(&loginok_payload, sizeof(sc_packet_login_ok));	//
	server->send_packet(session_id, loginok_packet);			// Ŭ�󿡰� ���� �˷��� ������
}

void MMOSession::response_login_failure()
{
	sc_packet_login_ok loginok_payload;  // �ֺ��� ������ ���� ������ // ���߿� DB ��û���� ����.... break;
	actor.get_login_packet_info(loginok_payload);						// dbó���� �񵿱�� �ٲ�� ��;

	loginok_payload.id = -1;

	c2::Packet* loginok_packet = c2::Packet::alloc();

	loginok_packet->write(&loginok_payload, sizeof(sc_packet_login_ok));	//

	server->send_packet(session_id, loginok_packet);			// Ŭ�󿡰� ���� �˷��� ������
}

void MMOSession::on_handling_db_task(DBTask* task)
{
	switch (task->type)
	{
		case LOAD_ACTOR:
		{
			LoadActorTask* load_task = reinterpret_cast<LoadActorTask*>(task);
			if (load_task->is_success == true)
			{
				actor.reset_data(load_task);

				enter_zone();
			}
			else // ID�� ������ ID ���� ������.
			{
				CreateActorTask* create_task = new CreateActorTask(this->session_id, load_task->name);
				
				g_db_manager->post_db_writing_task(create_task);
			}

			break;
		}
		case DTT_CREATE_ACTOR:
		{
			CreateActorTask* creation_task = reinterpret_cast<CreateActorTask*>(task);
			if (creation_task->is_success == true)
			{
				this->request_login_validation(actor.name);
				//actor.reset_data_when_creation(creation_task);
				//enter_zone();
				//this->request_updating_position(actor.y, actor.x);
			}
			else
			{
				response_login_failure();

				printf("Actor ���� ����...");
			}
		}
			break;
	}

	delete task;
}

void MMOSession::request_login_validation(char* name)
{
	LoadActorTask* load_actor_task = new LoadActorTask(this->session_id, name);
	
	g_db_manager->post_db_reading_task(load_actor_task);
}

void MMOSession::request_updating_position(int y, int x)
{
	UpdatePositionTask* task = new UpdatePositionTask(this->session_id, actor.user_id, y, x);
	
	g_db_manager->post_db_writing_task(task);
}


void MMOSession::enter_zone()
{
	MMOZone* zone = g_server->get_zone();

	this->response_loginok();		// �α��� ó�� �� ����

	zone->enter_actor(&actor);

	MMOSector*		current_sector = zone->get_sector(&actor);
	const MMONear*	nears = current_sector->get_near(actor.y, actor.x); // �ֺ� ���͵�.
	int				near_cnt = nears->count;


	std::unordered_map<int32_t, MMOActor*>	local_view_list;
	std::unordered_set<int32_t>				local_view_list_for_npc;

	for (int n = 0; n < near_cnt; ++n)
	{
		AcquireSRWLockShared(&nears->sectors[n]->lock); //sector�� �б� ���ؼ� ���� ��� 
		for (auto& other_iter : nears->sectors[n]->actors)
		{
			if (other_iter.second == &actor)
				continue;

			if (actor.is_near(other_iter.second) == true) // ��ó�� �´ٸ� ����.
				local_view_list.insert(other_iter);
		}

		//NPC ó�� ����.
		for (auto other_iter : nears->sectors[n]->npcs)
		{
			MMONpc* npc = g_npc_manager->get_npc(other_iter);
			if (actor.is_near(npc) == true) // ��ó�� �´ٸ� ����.
			{
				local_view_list_for_npc.insert(other_iter);			// �� npc�� �þ� ����Ʈ�� �־���.
			}
		}
		ReleaseSRWLockShared(&nears->sectors[n]->lock);
	}

	////////// �������� ���濡 ������ ���� ���� ������ �޴´�. 
	c2::Packet* my_info_packet = c2::Packet::alloc();				// �ֺ��� ������ ���� ������ 
	sc_packet_enter my_info_payload{ {sizeof(sc_packet_enter), S2C_ENTER}, (int16_t)this->session_id, {}, 0, actor.x , actor.y };
	memcpy(my_info_payload.name, actor.name, 50);
	my_info_packet->write(&my_info_payload, sizeof(sc_packet_enter));


	//my_info_packet->add_ref( local_view_list.size() );			// �� �ۿ��� �ϸ� ����  ����� ����Ǿ� ������ ��.
	for (auto& iter : local_view_list)
	{
		if (iter.second == &actor)
			continue;

		MMOActor* other = iter.second;
		actor.send_enter_packet_without_updating_viewlist(other);
		other->send_enter_packet(&actor, my_info_packet);
	}

	my_info_packet->decrease_ref_count();						// my_info_packet ������� 


////// NPC ó��.
	for (auto npc_id : local_view_list_for_npc)
	{
		MMONpc* npc = g_npc_manager->get_npc(npc_id);
		AcquireSRWLockExclusive(&npc->lock);						//�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
		npc->view_list.emplace(actor.get_id(), &actor);		// ���� �þ� ����Ʈ�� �־���.
		ReleaseSRWLockExclusive(&npc->lock);						//�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 

		local_timer->push_timer_task(npc->id, TTT_NPC_SCRIPT, 1, actor.session_id);

		// move������./// ������ npc ���� ������ �ϼ�. 
		actor.send_enter_packet_without_updating_viewlist(npc);
	}

	AcquireSRWLockExclusive(&actor.lock);							//�� view_list �� �����ϱ� �б� ���ؼ� ���� ��� 
	actor.view_list = local_view_list;
	actor.view_list_for_npc = local_view_list_for_npc;
	ReleaseSRWLockExclusive(&actor.lock);
}