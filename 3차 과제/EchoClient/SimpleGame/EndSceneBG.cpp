#include "stdafx.h"
#include "Renderer.h"
#include "GameObject.h"
#include "EndSceneBG.h"

EndSceneBG::EndSceneBG() : GameObject()
{
	this->set_hp(100);
	this->texture_id = this->renderer->GenPngTexture("./assets/clear.png");
}

EndSceneBG::~EndSceneBG()
{
}

void EndSceneBG::render()
{
	renderer->DrawGround(0.f, 0.f, 0.f // ÇÇº¿ÀÌ ¼¾ÅÍ
		, 1000.f, 1000.f, 0.f
		, 1, 1, 1, 1
		, this->texture_id
		, 1.f
	);
}

void EndSceneBG::update(float elpasedTime)
{
}

void EndSceneBG::collide(float elpasedTime)
{
}

void EndSceneBG::update_after_collision(float elpasedTime)
{
}
