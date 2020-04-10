#pragma once
#include "config.h"
#include "Renderer.h"
#include "GameObject.h"
#include "PhyxsManager.h"
#include "Sound.h"


class Scene;
class SceneManager
{
public:
	SceneManager(Renderer* renderer, Physics* physics, Sound* sound);
	~SceneManager();

	void update(float elapsed_time_in_sec);
	void collide(float elapsed_time_in_sec);
	void update_after_collision(float elapsed_time_in_sec);
	void do_garbage_collection();
	void render();
	void change_scene();

	Renderer*	get_renderer();
	Scene*		get_current_scene();

private:
	Physics*		physx;
	Renderer*		renderer;
	Sound*			sound;
	Scene*			scenes[ST_MAX];
	Scene*			current_scene;
	size_t			current_scene_index;
};
