#pragma once
#include "stdafx.h"
#include <iostream>
#include <windows.h> 
#pragma comment (lib, "winmm")
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"
#include "SceneManager.h"

class Framework
{
public:
	~Framework();
	bool initialize(int argc, char** argv);
	void run();
	void finalize();

	static void bind_scene_manager(SceneManager* scnmgr);

	static Renderer*		get_renderer();
	static Sound*			get_sound();
	static Physics*			get_physics();
	static SceneManager*	get_scene_manager();
	static int				get_bgm_index();
	static void				set_bgm_index(int bgmindex);
	static bool				get_has_foucs();
	static HWND				get_hwnd();

private:
	static void render(int temp);
	static void present(void);
	static void idle(void);


	static void mouse_input(int button, int state, int x, int y);
	static void keyDownInput(unsigned char key, int x, int y);
	static void keyUpInput(unsigned char key, int x, int y);
	static void special_key_down_input(int key, int x, int y);
	static void specialKeyUpInput(int key, int x, int y);

private:
	static Renderer*		renderer;
	static Sound*			sound;
	static Physics*			physics;
	static SceneManager*	scene_manager;
	static int				current_bgm_index;
	static bool				has_focus;
	static HWND				hwnd;
};

