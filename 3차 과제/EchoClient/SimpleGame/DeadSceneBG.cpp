#include "stdafx.h"
#include "Renderer.h"
#include "GameObject.h"
#include "DeadSceneBG.h"

DeadSceneBG::DeadSceneBG() : GameObject()
{
	this->set_hp(100);
	this->texture_id = this->renderer->GenPngTexture("./assets/gameover.png");
}

DeadSceneBG::~DeadSceneBG()
{
}

void DeadSceneBG::render()
{
	renderer->DrawGround(0.f, 0.f, 0.f // ÇÇº¿ÀÌ ¼¾ÅÍ
		, 1000.f, 1000.f, 0.f
		, 1, 1, 1, 1
		, this->texture_id
		, 1.f
	);
}

void DeadSceneBG::update(float elpasedTime)
{
}

void DeadSceneBG::collide(float elpasedTime)
{
}

void DeadSceneBG::update_after_collision(float elpasedTime)
{
}
