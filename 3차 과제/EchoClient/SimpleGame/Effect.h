#pragma once
#include "GameObject.h"

class Effect : public GameObject
{
	Effect();
	virtual ~Effect();

	virtual void render()									override final;
	virtual void update(float elpasedTime)					override final;
	virtual void collide(float elpasedTime)					override final;
	virtual void update_after_collision(float elpasedTime)	override final;
	virtual void on_collision(GameObject*)					override final;
};

