#pragma once

#include "../C2Server/C2Server/Session.h"
#include "MMOActor.h"

class MMOSession : public Session
{
public :
	MMOSession();
	~MMOSession();

private:
	MMOActor actor;
};

