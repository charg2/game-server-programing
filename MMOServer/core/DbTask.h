#pragma once
#include <cstdint>

enum DBTaskType
{
	LOAD_ACTOR,
	DTT_UPDATE_ACTOR_POSITION = 0,

	DTT_CREATE_ACTOR,
};


struct DBTask
{
	DBTask();
	DBTask(const DBTask& other) = delete;
	DBTask(DBTask&& other) = delete;
	virtual ~DBTask();

	DBTaskType	type;
	uint64_t	session_id;
	bool		is_success;
};
