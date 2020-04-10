#include "stdafx.h"
#include "Renderer.h"
#include "GameObject.h"
#include "TitleBG.h"

TitleBG::TitleBG() : GameObject(), current_texture{ -1 }, title{ -1 }, title1{ -1 }
{
	this->set_hp(100);
	
	this->title = this->renderer->GenPngTexture("./assets/title.png");

	current_texture = title;
}

TitleBG::~TitleBG()
{
}

void TitleBG::render()
{
	renderer->DrawGround(0.f, 0.f, 0.f // ÇÇº¿ÀÌ ¼¾ÅÍ
		, 1000.f, 1000.f, 0.f
		, 1, 1, 1, 1
		, this->current_texture
		, 1.f
	);
}

void TitleBG::update(float elpasedTime)
{
}

void TitleBG::collide(float elpasedTime)
{
}

void TitleBG::update_after_collision(float elpasedTime)
{
}

void TitleBG::on_collision(GameObject*)
{
}
