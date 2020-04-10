#include "stdafx.h"
#include "Layer.h"
#include "Scene.h"
#include "Framework.h"
#include "Renderer.h"
#include "GameObject.h"
#include "KeyManager.h"
#include "HittedComponent.h"
#include "../../Common/protocol.h"
#include "OtherPlayer.h"

#ifdef OBJECT_POOL_ON
c2::ObjectPool<OtherPlayer>		OtherPlayer::other_pool			{};
#endif

int								OtherPlayer::shared_texture_id	{ -1 };

OtherPlayer::OtherPlayer()
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

#ifdef OBJECT_POOL_ON
	{}

#else
	if (-1 == OtherPlayer::shared_texture_id )//&& nullptr != Framework::get_renderer() )
	{
		OtherPlayer::shared_texture_id = Framework::get_renderer()->GenPngTexture("./assets/queen.png");
	}
#endif
}

OtherPlayer::~OtherPlayer()
{
}

void OtherPlayer::render()
{
	float x = this->x * 125 + 70;
	float y = this->y * 125;
	float z = 1;

	float sz = 1 * 100.f;
	float sx = 1 * 100.f;
	float sy = 1 * 100.f;

	//(this->current_animation_x += 1) %= 3;
	this->renderer->DrawTextureRect
	(
		x
		, y
		, z
		, sx, sy, sz
		, this->color.r
		, this->color.g
		, this->color.b
		, this->color.a
		, OtherPlayer::shared_texture_id
	);
}

void OtherPlayer::update(float elpased_time)
{
}

void OtherPlayer::collide(float elpasedTime)
{}

void OtherPlayer::update_after_collision(float elpasedTime)
{
}

void OtherPlayer::on_collision(GameObject* other)
{
	//OtherPlayer::alloc();
}

#ifdef OBJECT_POOL_ON
bool OtherPlayer::init()
{
	OtherPlayer::shared_texture_id = Framework::get_renderer()->GenPngTexture("./assets/other.png");

	return other_pool.init();
}

bool OtherPlayer::fin()
{
	return false;
}

OtherPlayer* OtherPlayer::allocate()
{
	return other_pool.alloc();
}

void OtherPlayer::free(OtherPlayer* obj)
{
	other_pool.free(obj);
}
#else
bool OtherPlayer::init()
{
	return true;
}

bool OtherPlayer::fin()
{
	return true;
}
OtherPlayer* OtherPlayer::allocate()
{
	return new OtherPlayer;
}

void OtherPlayer::free(OtherPlayer* obj)
{
	delete obj;
}
#endif