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

	void move_to(int y, int x, char direction);

	void response_loginok();
	void response_login_failure();

	virtual void on_handling_db_task(DBTask* task);

	void request_login_validation(const wchar_t* w_name);
	void request_updating_position(int y, int x);
	void request_updating_all(int y, int x, int hp, int level, int exp);
	void request_change_status(int hp, int level, int exp);

	//void has_vliad_id();

private:
	MMOActor	actor;
};

