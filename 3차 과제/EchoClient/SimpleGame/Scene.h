#pragma once

#include "stdafx.h"
#include <vector>
#include <string>

class Layer;
class SceneManager;
class Scene
{ 
public:
	Scene(SceneManager* scenemanager, std::wstring&& name);
	Scene(const Scene&) = delete;
	Scene(Scene&&)		= delete;
	virtual ~Scene();

	virtual void	on_change_scene();

	void			update(float delta_time);
	void			collide(float delta_time);
	void			update_after_collision(float delta_time);
	void			do_garbage_collection();
	void			render();

	void			add_layer(Layer* layer);
	void			remove_layer(Layer* layer);
	Layer*			find_layer(std::wstring& layer_name);
	
	std::wstring&	get_name();
	SceneManager*	get_scene_manager();
	Layer*			get_layer(std::wstring* layer_name);

	void			set_name(std::wstring& new_name);
	void			set_scene_manager(SceneManager* scene_mgr);

	static uint64_t get_scene_index();
	static void		set_scene_index(uint64_t scene_index);

protected:
// unity�� ��� Lyaer�� �ִ� 32�� ���� �����ϴµ�.
	std::vector<Layer*> layers; // ���� ���۽� Scene�� �ְ� ���� �ٲ��� ���� �༮��.
	Tag					tag;
	std::wstring		name;
	SceneManager*		scene_manager;

	static uint64_t		scene_index;
};

