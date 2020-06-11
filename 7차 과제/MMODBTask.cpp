#include <memory>
#include "MMODBTask.h"

UpdatePositionTask::~UpdatePositionTask()
{
}

CreateActorTask::~CreateActorTask()
{
}




LoadActorTask::LoadActorTask(uint64_t session_id, char* name) :  user_id{}, y{}, x{}, level{}, hp{}, exp{}
{
	this->type = DBTaskType::LOAD_ACTOR;
	this->session_id = session_id;
	this->is_success = false;
	
	memcpy(this->name, name, 50);
}


LoadActorTask::~LoadActorTask()
{
}
