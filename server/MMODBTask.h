#pragma once
#include <cstdint>
#include "DbTask.h"

struct CreateActorTask : public DBTask
{
	CreateActorTask(uint64_t session_id, const wchar_t* name, int y, int x);
	virtual ~CreateActorTask();

	wchar_t		name[50];

// result
	int32_t		user_id;
	int32_t		y, x;
	//int32_t		hp;
	//int32_t		exp;
	//int8_t		level;

	int8_t		reason;
};

struct UpdatePositionTask : public DBTask
{
	UpdatePositionTask(uint64_t session_id, int32_t user_id, int32_t y, int32_t x);
	virtual ~UpdatePositionTask();

	
	int32_t		user_id;
	int32_t		y, x;

	// no result
};

struct LoadActorTask : public DBTask
{
	LoadActorTask(uint64_t session_id, const wchar_t* name);
	LoadActorTask(const LoadActorTask& other) = delete;
	virtual ~LoadActorTask();

	wchar_t name[50];		// id

	int32_t user_id;
	int32_t y, x;
	int32_t level;
	int32_t hp;
	int32_t exp;
};


struct ChangeStatTask : public DBTask
{
	ChangeStatTask(uint64_t session_id, int32_t user_id, int32_t level, int32_t hp, int32_t exp);
	ChangeStatTask(const LoadActorTask& other) = delete;
	virtual ~ChangeStatTask();

	int32_t user_id;
	int32_t level;
	int32_t hp;
	int32_t exp;
};


struct UpdateAllTask : public DBTask
{
	UpdateAllTask(uint64_t session_id, int32_t user_id, int32_t y, int32_t x, int32_t level, int32_t hp, int32_t exp);
	virtual ~UpdateAllTask();


	int32_t		user_id;
	int32_t		y, x;
	int32_t		level;
	int32_t		hp;
	int32_t		exp;
};