#pragma once

#include "../C2Server/C2Server/OuterServer.h"
#include "MMOSession.h"
#include "MMOZone.h"

class MMOServer final : public OuterServer
{
public:
	// OuterServer을(를) 통해 상속됨
	MMOServer();
	virtual ~MMOServer();


	void init_simulator();

	virtual void on_create_sessions(size_t n)		override final;
	virtual void on_connect(uint64_t session_id)	override final;
	virtual void on_disconnect(uint64_t session_id) override final;
	virtual bool on_accept(Session* session)		override final;
	virtual void on_wake_io_thread()				override final;
	virtual void on_sleep_io_thread()				override final;
	virtual void custom_precedure(uint64_t idx)		override final;
	virtual void on_update()						override final;
	virtual void on_start()							override final;

	MMOActor* getActor(uint64_t session_id);
	MMOZone* get_zone();
private:
	MMOZone* zone;
};

