#include "stdafx.h"
#include "Vector3.h"
#include "Physx.h"

Physx::Physx() : mass{ 0 }, velocity{ 0.f, 0.f, 0.f }, acceleration{ 0.f, 0.f, 0.f }, friction_coefficient{0.f}
{
}

Physx::Physx(const Physx& other)
	: mass{ other.mass }
	, velocity{ other.velocity.x, other.velocity.y, other.velocity.z }
	, acceleration{ other.acceleration.x, other.acceleration.y, other.acceleration.z }
	, friction_coefficient{ other.friction_coefficient }
{

}

void Physx::initialize()
{
	this->mass			= -1;

	this->velocity.x	= 0.f; // velocity
	this->velocity.y	= 0.f;
	this->velocity.z	= 0.f;
	 
	this->acceleration.x = 0.f; // acceleration
	this->acceleration.y = 0.f;
	this->acceleration.z = 0.f;

	this->friction_coefficient  = 0.f; //¸¶Âû °è¾¥friction co
}

Vector3& Physx::get_velocity()
{
	return this->velocity;
}

Vector3& Physx::get_acceleration()
{
	return this->acceleration;
}

const Vector3& Physx::get_cvelocity() const
{
	return this->velocity;
}

const Vector3& Physx::get_cacceleration() const
{
	return this->acceleration;
}

float Physx::get_frictioncoef()
{
	return this->friction_coefficient;
}

float Physx::get_mass()
{
	return this->mass;
}

void Physx::set_velocity(Vector3& velocity)
{
	this->velocity.x = velocity.x;
	this->velocity.y = velocity.y;
	this->velocity.z = velocity.z;
}

void Physx::set_velocity(float vx, float vy, float vz)
{
	this->velocity.x = vx;
	this->velocity.y = vy;
	this->velocity.z = vz;
}

void Physx::set_acceleration(Vector3& acceleration)
{
	this->acceleration.x = acceleration.x;
	this->acceleration.y = acceleration.y;
	this->acceleration.z = acceleration.z;
}

void Physx::set_acceleration(float ax, float ay, float az)
{
	this->acceleration.x = ax;
	this->acceleration.y = ay;
	this->acceleration.z = az;
}

void Physx::set_frictioncoef(float coef)
{
	this->friction_coefficient = coef;
}

void Physx::set_mass(float mass)
{
	this->mass = mass;
}
