#include "main.h"
#include "MMOSession.h"

MMOSession::MMOSession() : actor{ this }
{
}

MMOSession::~MMOSession()
{
}

MMOActor* MMOSession::get_actor()
{
	return &actor;
}
