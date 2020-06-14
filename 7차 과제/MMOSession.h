#pragma once

#include "core/Session.h"
#include "MMOActor.h"


struct DBTask;
class MMOSession : public Session
{
public :
	MMOSession();
	~MMOSession();

	MMOActor* get_actor();

	void enter_zone();

	void response_loginok();
	void response_login_failure();

	virtual void on_handling_db_task(DBTask* task);

	void request_login_validation(char* name);
	void request_updating_position(int y, int x);
	//void has_vliad_id();

private:
	MMOActor	actor;
};

