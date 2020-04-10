#include "EchoSession.h"
#include "EchoServer.h"

EchoServer::EchoServer() : OuterServer{}
{

}

EchoServer::~EchoServer()
{
}

bool EchoServer::on_initialize_after_init()
{
	return true;
}

bool EchoServer::on_initialize_before_init()
{
	return true;
}

void EchoServer::on_connect(Session* session)
{
}

void EchoServer::on_disconnect(Session* session)
{
	ByeResponse bye;

	bye.session_id = session->unique_session_id;

	broadcast_to_all( &bye ,session, false);
}

bool EchoServer::on_accept(Session* session)
{
	return true;
}


void EchoServer::on_finalize_after_fin()
{
}

void EchoServer::on_finalize_before_fin()
{
}

void EchoServer::on_update()
{
}

void EchoServer::broadcast_to_all(c2::Packet* out_packet, Session* caster, bool except_me /* = false */)
{
	if (false == except_me)
	{
		AcquireSRWLockShared(&srw_lock);

		size_t sessoin_cnt{ sessions.size() };
		for (size_t n{}; n < sessoin_cnt; ++n)
		{
			sessions[n]->pre_send(out_packet);
		}

		ReleaseSRWLockShared(&srw_lock);
	}
	else
	{
		AcquireSRWLockShared(&srw_lock);

		size_t sessoin_cnt{ sessions.size() };
		for (size_t n{}; n < sessoin_cnt; ++n)
		{
			if (caster != sessions[n])
			{
				sessions[n]->pre_send(out_packet);
			}
		}

		ReleaseSRWLockShared(&srw_lock);
	}
}

void EchoServer::broadcast_to_all(PacketHeader* out_payload, Session* caster, bool except_me)
{
	if (false == except_me)
	{
		AcquireSRWLockShared(&srw_lock);

		size_t sessoin_cnt{ sessions.size() };
		for (size_t n{}; n < sessoin_cnt; ++n)
		{
			sessions[n]->pre_send(out_payload);
		}

		ReleaseSRWLockShared(&srw_lock);
	}
	else
	{
		AcquireSRWLockShared(&srw_lock);

		size_t sessoin_cnt{ sessions.size() };
		for (size_t n{}; n < sessoin_cnt; ++n)
		{
			if (caster != sessions[n])
			{
				sessions[n]->pre_send(out_payload);
			}
		}

		ReleaseSRWLockShared(&srw_lock);
	}
}

void EchoServer::broadcast_around(c2::Packet* out_packet, Session* caster)
{
	EchoSession* echo_caster = (EchoSession*)caster;

	AcquireSRWLockShared(&srw_lock);
	
	size_t sessoin_cnt { sessions.size() };
	for (size_t n{}; n <  sessoin_cnt; ++n)
	{
		if (echo_caster->player->is_nearby(((EchoSession*)sessions[n])->player))
		{
			sessions[n]->pre_send(out_packet);
		}
	}

	ReleaseSRWLockShared(&srw_lock);
}

void EchoServer::broadcast_around(PacketHeader* out_payload, Session* caster)
{
}

Session* EchoServer::create_session()
{
	EchoSession* session = new EchoSession{};
	session->server = this;

	return session;
}
