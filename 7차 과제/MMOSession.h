#pragma once

#include "core/Session.h"
#include "MMOActor.h"

class MMOSession : public Session
{
public :
	MMOSession();
	~MMOSession();

	MMOActor* get_actor();
	void response_loginok();

private:
	MMOActor	actor;
};

