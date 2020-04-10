#include "stdafx.h"
//#include "Input.h"
#include "EchoClient.h"
#include "Framework.h"
SceneManager*	Framework::scene_manager		{ nullptr };
Sound*			Framework::sound				{ nullptr };
Renderer*		Framework::renderer				{ nullptr };
Physics*		Framework::physics				{ nullptr };
int				Framework::current_bgm_index	{ -1 };
bool			Framework::has_focus			{ false };
HWND			Framework::hwnd					{ NULL };

Framework::~Framework()
{
	//delete g_renderer;
	//g_renderer = nullptr;

	delete physics;
	physics = nullptr;

	delete sound;
	sound = nullptr;
}

bool Framework::initialize(int argc, char** argv)
{
	timeBeginPeriod(1);

	// Initialize GL things
	glutInit(&argc, argv);
	// 더블 버퍼링.
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	//
	glutInitWindowPosition( 0, 0); // 창 시작 위치
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Game Software Engineering KPU");

	glewInit();
	if (glewIsSupported("GL_VERSION_3_0"))
		std::cout << " GLEW Version is 3.0\n ";
	else
		std::cout << "GLEW 3.0 not supported\n ";

	sound			= new Sound();
	physics			= new Physics();
	renderer		= new Renderer(1000, 1000);

	HWND z = (HWND)-1;
	HWND zz = NULL;
	this->hwnd = FindWindowEx((HWND)GetParent(z), zz, L"FREEGLUT", NULL);
	printf("FindWindows:: %d\n", (int)hwnd);


	if (false == renderer->IsInitialized())
	{
		std::cout << "Renderer could not be initialized.. \n";
	}
	scene_manager	= new SceneManager(renderer, physics, sound);
	glutDisplayFunc(present);

	glutKeyboardFunc(keyDownInput); // 눌리는 순가.
	glutKeyboardUpFunc(keyUpInput); // 떼는 순간.
	glutSpecialFunc(special_key_down_input);
	glutSpecialUpFunc(specialKeyUpInput);

	glutMouseFunc(mouse_input);
	//prev_time = glutGet(GLUT_ELAPSED_TIME);
	glutTimerFunc(16, render, NULL);


	return true;
}


void Framework::run()
{
	glutMainLoop();
}

void Framework::finalize()
{
	delete scene_manager;
	scene_manager = nullptr;

	timeEndPeriod(1);
}

void Framework::bind_scene_manager(SceneManager* scnmgr)
{
}

void Framework::render(int temp)
{
	static int	prev_time				= glutGet(GLUT_ELAPSED_TIME);
	int			current_time			= glutGet(GLUT_ELAPSED_TIME);
	int			elapsed_time			= current_time - prev_time;
	float		elapsed_time_in_sec		= (float)elapsed_time / 1000.0f;

	prev_time							= current_time;

	// game loop

	g_echo_client->update();
	scene_manager->update(elapsed_time_in_sec);
	//global_plyaer->update(elapsed_time_in_sec);
	scene_manager->collide(elapsed_time_in_sec);
	scene_manager->update_after_collision(elapsed_time_in_sec);
	scene_manager->do_garbage_collection();
	scene_manager->render();
	scene_manager->change_scene(); 

	glutSwapBuffers();
	glutTimerFunc(10, render, NULL);
}



void Framework::present(void)
{
}

void Framework::idle(void)
{
}

void Framework::mouse_input(int button, int state, int x, int y)
{
}

void Framework::keyDownInput(unsigned char key, int x, int y)
{
	//if ('w' == key || 'W' == key)
	//{
	//	Input::set_key_down(KeyCode::W, true);
	//	Input::set_key_up(KeyCode::W, false);
	//}

	//if ('a' == key || 'A' == key)
	//{
	//	Input::set_key_down(KeyCode::A, true);
	//	Input::set_key_up(KeyCode::A, false);
	//}
	//if ('s' == key || 'S' == key)
	//{
	//	Input::set_key_down(KeyCode::S, true);
	//	Input::set_key_up(KeyCode::S, false);
	//}
	//if ('d' == key || 'D' == key)
	//{
	//	Input::set_key_down(KeyCode::D, true);
	//	Input::set_key_up(KeyCode::D, false);
	//}

	//if (' ' == key)
	//{
	//	Input::set_key_down(KeyCode::SPACE, true);
	//	Input::set_key_up(KeyCode::SPACE, false);
	//}
}

void Framework::keyUpInput(unsigned char key, int x, int y)
{
	//if ('w' == key || 'W' == key)
	//{
	//	Input::set_key_up(KeyCode::W, true);
	//	Input::set_key_down(KeyCode::W, false);
	//}

	//if ('a' == key || 'A' == key)
	//{
	//	Input::set_key_up(KeyCode::A, true);
	//	Input::set_key_down(KeyCode::A, false);
	//}
	//if ('s' == key || 'S' == key)
	//{
	//	Input::set_key_up(KeyCode::S, true);
	//	Input::set_key_down(KeyCode::S, false);
	//}
	//if ('d' == key || 'D' == key)
	//{
	//	Input::set_key_up(KeyCode::D, true);
	//	Input::set_key_down(KeyCode::D, false);
	//}

	//if (' ' == key)
	//{
	//	Input::set_key_up(KeyCode::SPACE, true);
	//	Input::set_key_down(KeyCode::SPACE, false);
	//}

}

void Framework::special_key_down_input(int key, int x, int y)
{
	/*if (GLUT_KEY_UP == key)
	{
		Input::set_key_down(KeyCode::UP, true);
	}

	if (GLUT_KEY_LEFT == key)
	{
		Input::set_key_down(KeyCode::LEFT, true);
	}
	if (GLUT_KEY_RIGHT == key)
	{
		Input::set_key_down(KeyCode::RIGHT, true);
	}
	if (GLUT_KEY_DOWN == key)
	{
		Input::set_key_down(KeyCode::DOWN, true);
	}*/
}

void Framework::specialKeyUpInput(int key, int x, int y)
{
	/*if (GLUT_KEY_UP == key)
	{
		Input::set_key_down(KeyCode::UP, false);
	}

	if (GLUT_KEY_LEFT == key)
	{
		Input::set_key_down(KeyCode::LEFT, false);
	}
	if (GLUT_KEY_RIGHT == key)
	{
		Input::set_key_down(KeyCode::RIGHT, false);
	}
	if (GLUT_KEY_DOWN == key)
	{
		Input::set_key_down(KeyCode::DOWN, false);
	}*/
}

Renderer* Framework::get_renderer()
{
	return Framework::renderer;
}

Sound* Framework::get_sound()
{
	return Framework::sound;
}

Physics* Framework::get_physics()
{
	return Framework::physics;
}

SceneManager* Framework::get_scene_manager()
{
	return Framework::scene_manager;
}

int Framework::get_bgm_index()
{
	return Framework::current_bgm_index;
}

void Framework::set_bgm_index(int bgmindex)
{
	Framework::current_bgm_index = bgmindex;
}

bool Framework::get_has_foucs()
{
	return has_focus;
}

HWND Framework::get_hwnd()
{
	return hwnd;
}


