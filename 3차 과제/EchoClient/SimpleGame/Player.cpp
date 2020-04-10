#include "stdafx.h"
#include "Layer.h"
#include "Scene.h"
#include "Framework.h"
//#include "Input.h"
#include "Renderer.h"
#include "GameObject.h"
#include "KeyManager.h"
#include "HittedComponent.h"
#include "../../Common/protocol.h"
#include "Player.h"

MainPlayer* g_player{ nullptr };

MainPlayer::MainPlayer() : GameObject()
{
	this->tag = TAG_MAIN_PLAYER;
	this->physx = new Physx();
	this->transform.scale = { 1, 1, 1 };
	this->hit_component = new HitComponent{ this, 1.0f };
	transform.set_position(1.95, 1.25, 0);
	transform.set_size(0.9f, .9f, 0.9f);
	set_color(1.0f, 1.0f, 1.0f, 1.0f);
	set_velocity(0.f, 0.f, 0.f);
	this->physx->set_mass(2.f); // 1kg
	set_hp(100); // 1kg
	health_point = 100;
	max_health_point = 100;
	set_FricCoef(0.7); // 1kg

	x = y = z = 0;
	this->texture_id = this->renderer->GenPngTexture("./assets/queen.png");
	//this->shot_sound = Framework::get_sound()->CreateShortSound("./assets/sounds/throwing.mp3");
}

MainPlayer::~MainPlayer()
{
}

void MainPlayer::render()
{
	float x = this->x * 125 + 70;
	float y = this->y * 125;
	float z = this->z * 125;


	float sz = transform.scale.z * 100.f;
	float sx = transform.scale.x * 100.f;
	float sy = transform.scale.y * 100.f;

	//(this->current_animation_x += 1) %= 3;
	this->renderer->DrawTextureRect
	(
		x
		, y
		, z
		, sx, sy, sz

		, 1
		, 1
		, this->color.b
		, this->color.a
		, this->texture_id
	);

}

void MainPlayer::update(float elapsed_time)
{
	auto forground_handle = GetForegroundWindow();
	if (Framework::get_hwnd() == forground_handle || GetConsoleWindow() == forground_handle)
	{
		MoveRequest request;
		request.session_id = g_echo_client->get_session_id();

		if (g_key_input->keyDown(VirtualKey::UpArrow))
		{
			request.direction = VirtualKey::UpArrow;
			g_echo_client->get_session()->pre_send(&request);
		}

		if (g_key_input->keyDown(VirtualKey::DownArrow))
		{
			request.direction = VirtualKey::DownArrow;
			g_echo_client->get_session()->pre_send(&request);
		}

		if (g_key_input->keyDown(VirtualKey::LeftArrow))
		{
			request.direction = VirtualKey::LeftArrow;
			g_echo_client->get_session()->pre_send(&request);
		}

		if (g_key_input->keyDown(VirtualKey::RightArrow))
		{
			request.direction = VirtualKey::RightArrow;
			g_echo_client->get_session()->pre_send(&request);
		}


		//if (g_key_input->keyDown(VirtualKey::Q) || g_key_input->keyDown(VirtualKey::ESC))
		//{
		//	ByeRequest request;
		//	request.session_id = g_echo_client->get_session_id();
		//	g_echo_client->get_session()->pre_send(&request);
		//}


		if (g_key_input->keyDown(VirtualKey::V))
		{
			HANDLE hProcess;
			DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(),
				&hProcess, 0, TRUE, DUPLICATE_SAME_ACCESS);
			printf("GetForegroundWindow:: %d \n\n ", (int)GetForegroundWindow());
			printf("GetActiveWindow:: %d \n\n ", (int)GetActiveWindow());
			printf("GetCurrentProcess:: %d \n\n ", (int)GetCurrentProcess());
			printf("GetConsoleWindow:: %d \n\n ", (int)GetConsoleWindow());
			printf("DuplicateHandle:: %d\n", (int)hProcess);

			HWND z = (HWND)-1;
			HWND zz = NULL;
			zz = FindWindowEx((HWND)GetParent(z), zz, L"FREEGLUT", NULL);
			printf("FindWindows:: %d\n", (int)zz);
			//zz = FindWindowEx((HWND)GetParent(z), zz, L"FREEGLUT", NULL);
			//printf("FindWindows:: %d\n", (int)zz);
			//zz = FindWindowEx((HWND)GetParent(z), zz, L"FREEGLUT", NULL);
			//printf("FindWindows:: %d\n", (int)zz);


			//while ((int)FindWindowEx((HWND)hProcess, NULL, , NULL))
			{

			}
		}
	}
}

void MainPlayer::collide(float elpasedTime)
{}

void MainPlayer::update_after_collision(float elpased_time)
{
	//this->remiainging_cooltime -= elpased_time;

	//Physx* physx = this->physx;
	//Vector3& velocity = physx->get_velocity();
	//float		mass = physx->get_mass();

	//// 방향과 크기를 가지는 벡터를 구할수잇다.
	//float vel_size = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);  // 
	//if (0.f < vel_size)
	//{
	//	float vx = velocity.x / vel_size;
	//	float vy = velocity.y / vel_size;

	//	///// apply friction //////////////////
	//	// 마찰력의 크기를 구해야함.
	//	float normal_force = mass * c2::konstant::GRIVITY; // 스칼라갑 크기를 의미 //수직항력 * 마찰게수 // 수직 항력의 사이즈
	//	// 운동 마찰력 구하기
	//	float fric_force = physx->get_frictioncoef() * normal_force; // 마찬가지로 스칼라. 마찰력의 크기를 구함.
	//	// 방향을 구한다음 마찰력의 크기를 곱(구??)해주면1

	//	float fx = -vx * fric_force;
	//	float fy = -vy * fric_force;
	//	/// 힘을 알아 냇드니 가속도를 구할 수 있음.
	//	// 힘을 받아으니 가속도는 질랴으로 나눠서 구함.
	//	// calculate acc from friction
	//	float f_accX = fx / mass;
	//	float f_accY = fy / mass;

	//	// 가속도가 주어졋다... 곧 이걸로 속도가 변한다.
	//	//속도는 가속도 곱하기 시간 
	//	float new_vel_x = velocity.x + f_accX * elpased_time;
	//	float new_vel_y = velocity.y + f_accY * elpased_time;

	//	if (new_vel_x * velocity.x < 0.f)
	//	{
	//		velocity.x = 0.f;
	//	}
	//	else
	//	{
	//		velocity.x = new_vel_x;
	//	}

	//	if (new_vel_y * velocity.y < 0.f)
	//	{
	//		velocity.y = 0.f;
	//	}
	//	else
	//	{
	//		velocity.y = new_vel_y;
	//	}

	//}


// 속도 갱신....
	//this->transform.position.x = this->transform.position.x + velocity.x * elpased_time;
	//this->transform.position.y = this->transform.position.y + velocity.y * elpased_time;

	// 바닥 처리.
	//if (FLT_EPSILON > this->transform.position.z) // 당을 꿀고 가기 직점.
	//{
	//	this->transform.position.z = 0.0f;
	//	velocity.z = 0.f;
	//}

	// 만약에 맵을 안나가게 ㅇㅇ;
	/*this->y = Clamp<int>(-4, this->y, +3);
	this->x = Clamp<int>(-4, this->x, +3);*/
	this->renderer->SetCameraPos(0, 0);
	//float time_per_action = .5f;
	//float action_per_time = 1.0 / time_per_action;
	//float frames_per_action = 8;

	//this->animation_timer += elpased_time;
	//this->animation_timer /= 3;
	//this->current_animation_x = animation_timer;
	//printf("%d %f", this->current_animation_x, this->animation_timer);

		// 체력이 0이면 죽음 
	//if (0 >= this->health_point)
	//{
	//	this->state = State::DEAD;
	//	//this->current_scene->set_scene_index(ST_DEAD);
	//}
}

void MainPlayer::on_collision(GameObject* other)
{
	if (TAG_ENEMY == other->get_tag())
	{
		if (true == this->can_hitted())
		{
			this->hit();
			float damage = other->get_damage();
			calc_damage(damage);
		}
	}
}

void MainPlayer::move_using_order(char order)
{
	printf("order : %d\n", order);

	//float fx, fy, fz;
	//this->get_position(&fx, &fy, &fz);

	if (order == VirtualKey::UpArrow)
	{
		this->y += 1;
	}
	else if (order == VirtualKey::DownArrow)
	{
		this->y -= 1;
	}
	else if (order == VirtualKey::LeftArrow)
	{
		this->x -= 1;
	}
	else if (order == VirtualKey::RightArrow)
	{
		this->x += 1;
	}
}

void MainPlayer::move_using_response(MoveResponse& response)
{
	printf("Player::move_using_response(MoveResponse& response)\n");
	
	this->x = response.x;
	this->y = response.y;
}

