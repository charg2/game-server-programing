#pragma once
#include <cstdint>

enum DBTaskType
{
	DTT_LOAD_ACTOR,
	DTT_UPDATE_ACTOR_POSITION = 0,
	DTT_UPDATE_ALL,
	DTT_CREATE_ACTOR,
	DTT_CHANGE_STAT,
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
