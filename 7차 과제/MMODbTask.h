#pragma once
#include <cstdint>
#include "DbTask.h"

struct CreateActorTask : public DbTask
{
	char name[50];

	uint64_t	user_id;
	int32_t		x, y;
	int32_t		hp;
	int32_t		exp;
	int8_t		level;
};

struct UpdatePositionTask : public DbTask
{
	uint64_t	user_id;
	int32_t		x, y;
};

struct LoadActorTask : public DbTask
{
	char name[50];		// id

	uint64_t user_id;
};
