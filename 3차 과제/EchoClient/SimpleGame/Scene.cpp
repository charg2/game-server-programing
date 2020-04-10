#include "stdafx.h"
#include "Scene.h"
#include "Layer.h"

uint64_t Scene::scene_index { ST_GAME1 };

Scene::Scene(SceneManager* scenemanager, std::wstring&& name = L"default")
	: tag{ TAG_SCENE }, name{ name }, scene_manager{ scenemanager }
{
}

Scene::~Scene()
{
	// 여기서 다 해방시켜줘야하나.
	for (Layer* layer : layers)
	{
		delete layer;
	}
}

void Scene::update(float delta_time)
{
	for (Layer* layer : layers)
	{
		layer->update(delta_time);
	}
}

void Scene::collide(float delta_time)
{
	for (Layer* layer : layers)
	{
		layer->collide(delta_time);
	}

	// 복수개의 레이어 충돌처리 해줘야 하나;
	// 복수개의 레이어 충돌 처리할때 그냥 레이어별로객체얻어서 하면 될듯.


}

void Scene::update_after_collision(float delta_time)
{
	for (Layer* layer : layers)
	{
		layer->update_after_collision(delta_time);
	}
}

void Scene::do_garbage_collection()
{
	for (Layer* layer : layers)
	{
		layer->do_garbage_collection();
	}
}

void Scene::render()
{
	for (Layer* layer : layers)
	{
		layer->render();
	}
}

void Scene::on_change_scene() // virtual 
{}

void Scene::add_layer(Layer* layer)
{
	layer->set_scene(this);
	this->layers.push_back(layer);
}

void Scene::remove_layer(Layer* layer)
{
	for (Layer* layer : layers)
	{
		layer->render();
	}
}

Layer* Scene::find_layer(std::wstring& layer_name)
{
	for (Layer* layer : layers)
	{
		if (layer_name == layer->get_name())
		{
			return layer;
		}
	}

	return nullptr;
}

std::wstring& Scene::get_name()
{
	return this->name;
}

void Scene::set_name(std::wstring& new_name)
{
	this->name = new_name;
}

uint64_t Scene::get_scene_index()
{
	return scene_index;
}

void Scene::set_scene_index(uint64_t scn_idx)
{
	Scene::scene_index = scn_idx;
}

SceneManager* Scene::get_scene_manager()
{
	return this->scene_manager;
}

Layer* Scene::get_layer(std::wstring* name)
{
	for (Layer* layer : layers)
	{
		if (layer->get_name() == name->c_str())
			return layer;
	}

	return nullptr;
}


void Scene::set_scene_manager(SceneManager* scene_mgr)
{
	this->scene_manager = scene_mgr;
}

