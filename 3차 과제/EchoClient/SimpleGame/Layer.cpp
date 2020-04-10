#include "stdafx.h"
#include "Scene.h"
#include "Layer.h"
#include "Framework.h"

#include <algorithm>

Layer::Layer(Scene* scene) : tag{ TAG_LAYER }, name{ L"default" }, scene{ scene }, depth{ 0.0f }
{
}

Layer::Layer(const wchar_t* layer_name, Scene* scene) : tag{ TAG_LAYER }, name{ layer_name }, scene{ scene }, depth{ 0.0f }
{
}

Layer::~Layer()
{
	for (GameObject* gobj: objects)
	{
		delete gobj;
	}
}

void Layer::update(float delta_time)
{
	GameObject* gobj;
	for (size_t n = 0; n < objects.size(); ++n)
	{
		if( true == this->objects[n]->get_active())
			this->objects[n]->update(delta_time);
	}


}

void Layer::collide(float delta_time)
{
	//for (GameObject* gobj : objects)
	//	gobj->collide(delta_time);
	Physics*	physics = Framework::get_physics();
	Sound*		sound	= Framework::get_sound();

	size_t object_num = this->objects.size();

	for (size_t src = 0; src < object_num; ++src)
	{
		for (size_t dest = src + 1; dest < object_num; ++dest)
		{
			if (objects[src]->get_physx() != nullptr && objects[dest]->get_physx() != nullptr
				&& objects[src]->get_active() != false && objects[dest]->get_active() != false )
			{
				if (physics->is_overlapped(objects[src], objects[dest]))
				{
					if (this->objects[src]->is_parent(this->objects[dest]) == false && this->objects[dest]->is_parent(this->objects[src]) == false)
					{
						physics->process_collision(objects[src], objects[dest]);
						objects[src]->on_collision(objects[dest]);
						objects[dest]->on_collision(objects[src]);
					}
				}
			}
		}
	}
}

void Layer::update_after_collision(float delta_time)
{
	for (GameObject* gobj : objects)
	{
		if ( true == gobj->get_active() )
			gobj->update_after_collision(delta_time);
	}
}

void Layer::do_garbage_collection()
{
	// state가 dead면 죽음.
	// 따로 가비지 컬렉터를 만들고
	// state 가 dead면 가비지 컬렉터에서 넣어서 죽여버리자.
	// 예정
	//GC* gc = Framework::get_gc();
	//for (GameObject* gobj : objects)
	//{
	//	if (gobj->get_state() == Dead)
	//	{
	//		// 가비지 컬렉터 호출해서 삭제하게 만듬.
	//		// gc->do_garbage_collection(gobj);
	//	}
	//}

	for (GameObject* gobj : objects)
	{
		if (DEAD == gobj->get_state())
		{
			//std::cout << gobj->get_name().c_str() << std::endl;;
			this->remove_object(gobj);
		}
	}
}

void Layer::render()
{
	for (GameObject* gobj : objects)
	{
		gobj->render();
	}
}

Scene* Layer::get_scene()
{
	return this->scene;
}

std::wstring& Layer::get_name()
{
	return name;
}

void Layer::set_name(std::wstring& new_name)
{
	this->name = new_name;
}

void Layer::set_scene(Scene* scene) noexcept
{
	this->scene = scene;
}

GameObject* Layer::find_object(std::wstring& name)
{
	for (GameObject* gobj : objects)
	{
		if (name == gobj->get_name())
		{
			return gobj;
		}
	}

	return nullptr;
}

void Layer::add_object(GameObject* gobj)
{
	gobj->set_scene(this->scene);
	gobj->set_layer(this);

	this->objects.push_back(gobj);
}

void Layer::remove_object(GameObject* gobj)
{
	this->objects.erase(std::remove(objects.begin(), objects.end(), gobj), objects.end());
}

size_t Layer::get_object_num()
{
	return this->objects.size();
}

std::vector<GameObject*>* Layer::get_objects()
{
	return &this->objects;
}
