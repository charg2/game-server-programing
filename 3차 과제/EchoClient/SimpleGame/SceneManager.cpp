#include "stdafx.h"
#include "Player.h"
#include "Scene.h"
#include "Framework.h"
#include "GameScene1.h"
//#include "BossScene.h"
//#include "TitleScene.h"
//#include "EndScene.h"
//#include "DeadScene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Dependencies\freeglut.h"

static int text_idx = 0;
static int bg_texture = -1;
int bgm = -1;
int hit = -1;
int fire = -1;
int particle_idx = -1;
int particle_texture = -1;

SceneManager::SceneManager(Renderer* renderer, Physics* physics, Sound* sound)
	: /*objects{}, */physx{ physics }, renderer{ renderer }, sound{ sound }, current_scene{}, current_scene_index{ ST_GAME1 }
{
	this->scenes[ST_GAME1]  = reinterpret_cast<Scene*>(new GameScene1(this));

	this->current_scene = this->scenes[ST_GAME1];
	//global_plyaer = new Player;

	//particle_texture	= renderer->GenPngTexture("./assets/particle.png");
	//particle_idx		= renderer->CreateParticleObject
	//(
	//	10000, // 100개
	//	-500, -500, // z축 값 no
	//	500, 500,
	//	-5, -5, //파티클 사이즈 얼마만큼 하것이냐
	//	5, 5, 
	//	-20, -20, // 생성 당시 초기속도
	//	20, 20
	//);

	//bgm		= sound->CreateBGSound("./sound/bg.mp3");
	Framework::set_bgm_index(bgm);
	//this->sound->PlayBGSound(bgm, true, 1.f);
}

SceneManager::~SceneManager()
{
	delete this->renderer;
	this->renderer = nullptr;
	delete this->physx;
	this->physx = nullptr;
}

void SceneManager::render()
{
	// current_scene[scene_index]->render();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	this->current_scene->render();

	static float p_time = 0.f;
	p_time += 0.016f; // 중첩되 시간이 넘ㅇ강함.

////	 파티클 코드
//	this->renderer->DrawParticle
//	(	
//		particle_idx
//		, 0, 0, 0,
//		1, // 곱해주는 값.
//		1, 1, 1, 1,
//		-50, -50,
//		particle_texture,
//		1, p_time		
//	);
}

void SceneManager::update(float elapsed_time_in_sec)
{
	this->current_scene->update(elapsed_time_in_sec);
}

void SceneManager::collide(float elapsed_time_in_sec)
{
	 this->current_scene->collide(elapsed_time_in_sec);
}

void SceneManager::update_after_collision(float elapsed_time_in_sec)
{
	this->current_scene->update_after_collision(elapsed_time_in_sec);
}

void SceneManager::do_garbage_collection()
{
	this->current_scene->do_garbage_collection();
}

Renderer* SceneManager::get_renderer()
{
	return this->renderer;
}
Scene* SceneManager::get_current_scene()
{
	return this->current_scene;
}
void SceneManager::change_scene()
{
	// 씬 자체에서 변경이 가능.
	// 현재씬이 바꼇느지 체크.
	uint64_t scene_index = Scene::get_scene_index();
	if ( scene_index != this->current_scene_index )
	{
		// 초기화 할것인지
		this->current_scene_index = scene_index;
		// 종료 처리 할것인지.
		this->scenes[scene_index]->on_change_scene();
		this->current_scene = this->scenes[scene_index];
	}
}

