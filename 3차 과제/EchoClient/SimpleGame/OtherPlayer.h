#pragma once

#include "ObjectPool.h"

class GameObject;

#define OBJECT_POOL_ON

class OtherPlayer : public GameObject
{
public:
	OtherPlayer();
	virtual ~OtherPlayer();

	virtual void render()									override final;
	virtual void update(float elpasedTime)					override final;
	virtual void collide(float elpasedTime)					override final;
	virtual void update_after_collision(float elpasedTime)	override final;
	virtual void on_collision(GameObject* other)			override final;

	int8_t x, y, z;
	wchar_t nickname[16];
	uint64_t session_id;

	static bool init();
	static bool fin();

	static OtherPlayer* allocate();
	static void free(OtherPlayer* obj);

private:

#ifdef OBJECT_POOL_ON
	static c2::ObjectPool<OtherPlayer>	other_pool;
#endif

	static int							shared_texture_id;
};
