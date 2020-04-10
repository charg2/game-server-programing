#include "stdafx.h"
#include "Framework.h"
#include "Renderer.h"
#include "Layer.h"
#include "Scene.h"
#include "HittedComponent.h"
#include "GameObject.h"
#include <math.h>
#include <float.h>

std::unordered_map<std::wstring*, GameObject*>		GameObject::name_map	{};
std::unordered_multimap<Tag, GameObject*>			GameObject::group_map	{};

GameObject::GameObject() : transform{ }, active_self{ true }, parent{ nullptr }, name{ L"default" }, color{ 1, 1, 1, 1 }, tag{ TAG_NONE }, hit_component{ nullptr }, damage{ 0 }
{
	this->renderer  = Framework::get_renderer();
}



/// 미제작.
GameObject::GameObject(const GameObject& other)
{
	this->transform= other.transform;

	if(nullptr != other.physx)
		physx = new Physx(*other.physx);

	this->color = other.color;
	name		= other.name;
}

GameObject::GameObject(GameObject&& other) noexcept
{
	this->transform		= other.transform;
	this->color			= other.color;
	this->physx			= other.physx;

	this->active_self	= other.active_self;
	this->age			= other.age;
	this->tag			= other.tag;
	this->name			= other.name;
	
	other.physx			= nullptr;
	other.name.clear();
}


GameObject::GameObject(Transform& transform, FColor& color) : active_self{ true }, parent{ nullptr }, name{ L"default" }
{
	this->transform	= transform;
	this->color		= color;
}

GameObject::~GameObject()
{
	if(nullptr != this->physx )
		delete this->physx;
}

void GameObject::get_position(float *x, float *y, float *z)
{
	*x = this->transform.position.x;
	*y = this->transform.position.y;
	*z = this->transform.position.z;
}

void GameObject::get_velocity(float* vx, float* vy, float* vz)
{
	Vector3 velocity = this->physx->get_cvelocity();
	*vx = velocity.x;
	*vy = velocity.y;
	*vz = velocity.z;
}
void GameObject::get_acceleration(float* ax, float* ay, float* az)
{
	Vector3 acceleration = this->physx->get_cacceleration();
	*ax = acceleration.x;
	*ay = acceleration.y;
	*az = acceleration.z;
}

void GameObject::getColor(float *r, float *g, float *b, float *a)
{
	*r = this->color.r;
	*g = this->color.g;
	*b = this->color.b;
	*a = this->color.a;
}

FColor* GameObject::get_color()
{
	return &this->color;
}

Transform* GameObject::get_transform()
{
	return &this->transform;
}

Physx* GameObject::get_physx()
{
	return this->physx;
}

float GameObject::get_mass()
{
	return this->physx->get_mass();
}

void GameObject::getFricCoef(float* coef)
{
	*coef = this->physx->get_frictioncoef();
}

void GameObject::get_tag(Tag* obj_type)
{
	*obj_type = this->tag;
}

Tag GameObject::get_tag()
{
	return this->tag;
}

void GameObject::getTextureID(int* id)
{
	*id = this->texture_id;
}

void GameObject::setTextureID(int id)
{
	this->texture_id = id;
}

float GameObject::get_age()
{
	return this->age;
}

int GameObject::get_damage()
{
	return this->damage;
}

Layer* GameObject::get_layer()
{
	return this->current_layer;
}

State GameObject::get_state()
{
	return this->state;
}

float GameObject::get_hp()
{
	return this->health_point;
}

Renderer* GameObject::get_renderer() const
{
	return this->renderer;
}

void GameObject::set_hp(float hp)
{
	this->health_point = hp;
}

void GameObject::calc_damage(float damage)
{
	this->health_point -= damage;
}

void GameObject::set_position(float x, float y, float z)
{
	this->transform.position.x = x;
	this->transform.position.y = y;
	this->transform.position.z = z;
}

void GameObject::set_size(float sx, float sy, float sz)
{
	this->transform.size.x = sx;
	this->transform.size.y = sy;
	this->transform.size.z = sz;
}

void GameObject::set_scale(float scx, float scy, float scz)
{
	this->transform.set_scale(scx, scy, scz);
}

void GameObject::set_velocity(float vx, float vy, float vz)
{
	this->physx->set_velocity(vx, vy, vz);
}

void GameObject::set_acceleration(float ax, float ay, float az)
{
	this->physx->set_acceleration(ax, ay, az);
}

void GameObject::set_color(float r, float g, float b, float alpha)
{
	this->color.r = r;
	this->color.g = g;
	this->color.b = b;
	this->color.a = alpha;
}

void GameObject::set_mass(float mass)
{
	this->physx->set_mass(mass);
}

void GameObject::set_FricCoef(float coef)
{
	this->physx->set_frictioncoef(coef);
}

void GameObject::update(float elpased_time)
{}

void GameObject::collide(float elpasedTime)
{}



void GameObject::render()
{}

void GameObject::update_after_collision(float elpased_time)
{
	//this->remiainging_cooltime -= elpased_time;

	//Physx*		physx		= this->physx;
	//Vector3&	velocity	= physx->get_velocity();
	//float		mass		= physx->get_mass();
	//// 방향과 크기를 가지는 벡터를 구할수잇다.
	//float vel_size			= sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);  // 
	//if ( 0.f < vel_size )
	//{
	//	float vx = velocity.x / vel_size;
	//	float vy = velocity.y / vel_size;

	//	///// apply friction //////////////////
	//	// 마찰력의 크기를 구해야함.
	//	float normal_force = mass * GRIVITY; // 스칼라갑 크기를 의미 //수직항력 * 마찰게수 // 수직 항력의 사이즈
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
	//	velocity.z = velocity.z - GRIVITY * elpased_time; // 


	//////////////////////////////
	//// 속도 갱신....
	//this->transform->position.x = this->transform->position.x + velocity.x * elpased_time; // //+ (0.5 * acc_x * (eTime * eTime));
	//this->transform->position.y = this->transform->position.y + velocity.y * elpased_time;
	//this->transform->position.z = this->transform->position.z + velocity.z * elpased_time;
	//  
	//// 바닥 처리.
	//if (FLT_EPSILON > this->transform->position.z) // 당을 꿀고 가기 직점.
	//{
	//	this->transform->position.z = 0.0f;
	//	velocity.z = 0.f;
	//}
}

void GameObject::on_collision(GameObject* other)
{
}


void GameObject::set_type(Tag type)
{
	this->tag = type;
}

void GameObject::set_age(float time)
{
	this->age += time ;
}

void GameObject::set_state(State state)
{
	this->state = state;
}

void GameObject::set_parent(GameObject* parent)
{
	this->parent = parent;
}

void GameObject::set_scene(Scene* scn)
{
	this->current_scene = scn;
}

void GameObject::add_force(float x, float y, float z, float elapse_Time)
{
	float mass			= this->physx->get_mass();
	Vector3& velocity	= physx->get_velocity();

	float accX, accY, accZ = 0.f;

	accX = x / mass;
	accY = y / mass;
	accZ = z / mass;

	velocity.x = velocity.x + accX * elapse_Time;
	velocity.y = velocity.y + accY * elapse_Time;
	velocity.z = velocity.z + accZ * elapse_Time;
}


bool GameObject::is_parent(GameObject* obj)
{
	// 에외처리.
	// if obj == nullptr 
	return this->parent == obj;
}

bool GameObject::can_hitted()
{
	if (nullptr != hit_component)
		return this->hit_component->can_hitted();
	else
		return false;
}



void GameObject::set_active(bool active)
{
	this->active_self = active;
}

bool GameObject::get_active()
{
	return this->active_self;
}

std::wstring& GameObject::get_name()
{
	return this->name;
}

void GameObject::set_name(std::wstring& name)
{
	this->name = name;
}

void GameObject::set_layer(Layer* layer)
{
	this->current_layer = layer;
}

void GameObject::set_renderer(Renderer* renderer)
{
	this->renderer = renderer;
}

void GameObject::hit()
{
	if (nullptr != hit_component)
		hit_component->hit();
}

//GameObject* GameObject::make_object(const wchar_t* name)
//{
//	return ;
//}

GameObject* GameObject::find_object(std::wstring* name)
{
	auto ret_iter = name_map.find(name);

	if (name_map.end() == ret_iter)
		return nullptr;
	else
		return ret_iter->second;
}



bool GameObject::find_objects(Tag tag, std::list<GameObject*>& find_list)
{
	// 둘다 이터레이터를 반환  <begin , end> 형식으로 반환 하나보.
	auto ret_iter	= group_map.equal_range(tag);
	
	/*int n			= ret_iter.first->first;*/
	//GameObject* n	= ret_iter.first->second;

	if (ret_iter.first == ret_iter.second)
		return false;
	else
		for (; ret_iter.first != ret_iter.second; ++ret_iter.first)
		{
			find_list.push_back(ret_iter.first->second);
	 	}
	return true;
}

void GameObject::destroy(GameObject* gobj)
{
// unordered_map에서 삭제
	name_map.erase(&gobj->name);

// group_map에서 삭제
	//group_map.erase();
	auto ret_iter = group_map.equal_range(gobj->tag);

	for (; ret_iter.first != ret_iter.second; ++ret_iter.first)
	{
		if (gobj == ret_iter.first->second)
		{
			group_map.erase(ret_iter.first);
		}
	}
}


