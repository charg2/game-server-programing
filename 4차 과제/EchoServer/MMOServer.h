#pragma once

#include "../C2Server/C2Server/OuterServer.h"
#include "MMOSession.h"
//#include "MMOZone.h"


struct MMOZone;
class MMOServer final : public OuterServer
{
public:
	// OuterServer을(를) 통해 상속됨
	MMOServer();
	virtual ~MMOServer();

	void init_npcs();

	virtual void on_create_sessions(size_t n)				override final;
	virtual void on_connect(uint64_t session_id)			override final;
	virtual void on_disconnect(uint64_t session_id)			override final;
	virtual bool on_accept(Session* session)				override final;
	virtual void on_wake_io_thread()						override final;
	virtual void on_sleep_io_thread()						override final;
	virtual void custom_precedure(uint64_t idx)				override final;
	virtual void on_update()								override final;
	virtual void on_start()									override final;
	virtual void on_timer_service(const TimerTask& task)	override final;

	void create_npcs(size_t capacity);

	MMOActor* get_actor(uint64_t session_id);
	MMOZone*  get_zone();

private:
	MMOZone* zone;
};

