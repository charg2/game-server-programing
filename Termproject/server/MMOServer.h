#pragma once

#include "core/OuterServer.h"
#include "MMOSession.h"
//#include "MMOZone.h"

#define send_packet_macro(s_id, pack) g_server->send_packet(s_id, pack);


struct MMOZone;
class MMOServer final : public OuterServer
{
public:
	// OuterServer을(를) 통해 상속됨
	MMOServer();
	virtual ~MMOServer();

	void init_npcs();

	virtual void on_create_sessions(size_t n)					override final;
	virtual void on_connect(uint64_t session_id)				override final;
	virtual void on_disconnect(uint64_t session_id)				override final;
	virtual void on_wake_io_thread()							override final;
	virtual void on_sleep_io_thread()							override final;
	virtual void custom_precedure(uint64_t idx)					override final;
	virtual void on_update()									override final;
	virtual void on_start()										override final;
	virtual void on_timer_service(const TimerTask& task)		override final;
	virtual bool on_load_config(c2::util::JsonParser* parser)	override final;

	void create_npcs(size_t capacity);

	MMOActor* get_actor(uint64_t session_id);
	MMOZone*  get_zone();

private:
	MMOZone* zone;
};


extern inline MMOServer* g_server{};