#include "stdafx.h"
#include "Transform.h"


Transform::Transform() : position{}, size{}, rotation{}, scale{}
{
}

Transform::Transform(const Transform& other) : position{ other.position }, size{ other.size }, rotation{other.rotation}, scale{other.scale}
{
}

void Transform::initialize()
{
	position.x = 0;
	position.y = 0;
	position.z = 0;

	size.x = 0;
	size.y = 0;
	size.z = 0;

	scale.x = 0;
	scale.y = 0;
	scale.z = 0;
}

Vector3& Transform::get_position()
{
	return this->position;
}

Vector3& Transform::get_size()
{
	return this->size;
}

Vector3& Transform::get_scale()
{
	return this->scale;
}

void Transform::set_position(Vector3& other_position)
{
	this->position.x = other_position.x;
	this->position.y = other_position.y;
	this->position.z = other_position.z;
}

void Transform::set_position(float x, float y, float z)
{
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}

void Transform::set_size(Vector3& size)
{
	this->size.x = size.x;
	this->size.y = size.y;
	this->size.z = size.z;

}

void Transform::set_size(float x, float y, float z)
{
	this->size.x = x;
	this->size.y = y;
	this->size.z = z;

}

void Transform::set_scale(Vector3& scale)
{
	this->scale.x = scale.x;
	this->scale.y = scale.y;
	this->scale.z = scale.z;
}

void Transform::set_scale(float x, float y , float z)
{
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;
}

Vector3& Transform::get_rotation()
{
	return this->rotation;
}

void Transform::set_rotation(Vector3& rotation)
{
	this->rotation.x = rotation.x;
	this->rotation.y = rotation.y;
	this->rotation.z = rotation.z;

}

void Transform::set_rotation(float x, float y, float z)
{
	this->rotation.x = x;
	this->rotation.y = y;
	this->rotation.z = z;
}

