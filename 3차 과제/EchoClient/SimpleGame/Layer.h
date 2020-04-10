#pragma once
#include "stdafx.h"
#include <vector>
#include <string_view>

class Scene;
class Layer
{
public:
	Layer() = delete;
	Layer(Scene* scene);
	Layer(const wchar_t* layer_name, Scene* scene);
	~Layer();

	void update(float delta_time);
	void collide(float delta_time);
	void update_after_collision(float delta_time);
	void do_garbage_collection();
	void render();

	GameObject*		find_object(std::wstring& obj_name);
	void			add_object(GameObject* gobj);
	void			remove_object(GameObject* gobj);

	Scene*						get_scene();
	std::wstring&				get_name();
	size_t						get_object_num();
	std::vector<GameObject*>*	get_objects();
	
	void			set_name(std::wstring& new_name);
	void			set_scene(Scene* scene) noexcept;
	
private:
	std::vector<GameObject*> objects;
	std::wstring			 name;
	Tag						 tag;
	Scene*					 scene; 
	float					 depth;
};