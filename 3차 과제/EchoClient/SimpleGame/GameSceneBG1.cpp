#include "stdafx.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameSceneBG1.h"

GameSceneBG1::GameSceneBG1()
{
	this->set_hp(100);
	this->texture_id = this->renderer->GenPngTexture("./assets/bg.png");
	this->shading_texture_id = this->renderer->GenPngTexture("./assets/shading.png");
}

GameSceneBG1::~GameSceneBG1()
{
}

void GameSceneBG1::render()
{
	//백그라운드는 가장 먼저 그리는게 좋다.
//this->renderer;,
	renderer->DrawGround(0.f, 0.f, 0.f // 피봇이 센터
		, 1000.f, 1000.f, 0.f
		, 1, 1, 1, 1
		, this->texture_id
		, 1.f
	);
}

void GameSceneBG1::update(float elpasedTime)
{
}

void GameSceneBG1::collide(float elpasedTime)
{
}

void GameSceneBG1::update_after_collision(float elpasedTime)
{
}

void GameSceneBG1::on_collision(GameObject*)
{
}


