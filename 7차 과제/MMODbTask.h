#pragma once
#include <cstdint>
#include "DbTask.h"

struct CreateActorTask : public DBTask
{
	virtual ~CreateActorTask();

	char name[50];

	int32_t		user_id;
	int32_t		y, x;
	int32_t		hp;
	int32_t		exp;
	int8_t		level;

	int8_t		reason;
};

struct UpdatePositionTask : public DBTask
{
	virtual ~UpdatePositionTask();

	int32_t		user_id;
	int32_t		y, x;
};

struct LoadActorTask : public DBTask
{
	LoadActorTask(uint64_t session_id, char* name);
	LoadActorTask(const LoadActorTask& other) = delete;
	virtual ~LoadActorTask();

	char name[50];		// id

	int32_t user_id;
	int32_t y, x;
	int32_t level;
	int32_t hp;
	int32_t exp;
};
