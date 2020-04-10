#include "stdafx.h"
#include "Sound.h"
#include "Framework.h"
#include "Layer.h"
#include "Scene.h"
#include "GameSceneBG1.h"
#include "Player.h"
//#include "Monstro.h"
//#include "Obstacle.h"
//#include "Poop.h"
//#include "Door.h"
#include "GameScene1.h"
#include "SceneManager.h"


GameScene1::GameScene1(SceneManager* scenemanager)
	: Scene(scenemanager, L"game1")
{
	Layer* background_layer = new Layer(L"background", this);
	Layer* game_layer		= new Layer(L"game", this);

	GameObject* game_bg = new GameSceneBG1;
	background_layer->add_object(game_bg);

	//GameObject* obstacle	= new Obstacle;
	////GameObject* boss_door	= new Door(DOOR_TOP, ST_BOSS);
	//GameObject* poop1		= new Poop;
	//GameObject* poop2		= new Poop;
	//GameObject* poop3		= new Poop;
	//GameObject* poop4		= new Poop;

	//GameObject* monster		= new Monstro;
	//poop1->set_position(-1, 2.5, 0);
	//poop4->set_position( 1,  2.5 , 0);
	//poop2->set_position( 0, 2.5, 0);
	//poop3->set_position( 0, 1.5, 0);
	////game_layer->add_object(boss);

	g_player = new MainPlayer();
	game_layer->add_object(g_player);
	//game_layer->add_object(poop2);
	//game_layer->add_object(poop3);
	//game_layer->add_object(poop4);
	//game_layer->add_object(obstacle);
	//game_layer->add_object(boss_door);

	this->add_layer(background_layer);
	this->add_layer(game_layer);
}

GameScene1::~GameScene1()
{
}

void GameScene1::on_change_scene()
{
	Layer* layer = find_layer(std::wstring{ L"game" });
	layer->remove_object(g_player);
	layer->add_object(g_player);

	Framework::get_sound()->DeleteBGSound(Framework::get_bgm_index());
	//Framework::get_sound()->PlayBGSound(this->bgm, true, 1.f);
	Framework::set_bgm_index(this->bgm);
}



