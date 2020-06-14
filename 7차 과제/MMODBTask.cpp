#include <memory>
#include "MMODBTask.h"


UpdatePositionTask::UpdatePositionTask(uint64_t session_id, int32_t user_id, int32_t y, int32_t x)
{
	this->type = DBTaskType::DTT_UPDATE_ACTOR_POSITION;
	this->session_id = session_id;
	this->is_success = false;

	this->user_id	= user_id;
	this->y			= y;
	this->x			= x;
}

UpdatePositionTask::~UpdatePositionTask()
{
}


CreateActorTask::CreateActorTask(uint64_t session_id, const char* name)
	: user_id{},/* y{}, x{}, hp{}, exp{}, level{}, */reason{}
{
	this->type = DBTaskType::DTT_CREATE_ACTOR;
	this->session_id = session_id;
	this->is_success = false;

	memcpy(this->name, name, 50);
}

CreateActorTask::~CreateActorTask() {}




LoadActorTask::LoadActorTask(uint64_t session_id, char* name) 
	:  user_id{}, y{}, x{}, level{}, hp{}, exp{}
{
	this->type = DBTaskType::LOAD_ACTOR;
	this->session_id = session_id;
	this->is_success = false;
	
	memcpy(this->name, name, 50);
}


LoadActorTask::~LoadActorTask(){}
