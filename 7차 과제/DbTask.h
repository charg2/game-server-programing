#pragma once


enum DbTaskType
{
	LOAD_ACTOR,
	DTT_UPDATE_ACTOR_POSITION = 0,

	DTT_CREATE_ACTOR,
};


struct DbTask
{
	DbTask() = delete;

	DbTaskType	type;
	uint64_t	session_id;
	bool		need_result;
};
