#include "stdafx.h"
#include "GameObject.h"
#include "PhyxsManager.h"


Physics::Physics()
{
}

Physics::~Physics()
{
}

bool Physics::is_overlapped(GameObject* a, GameObject* b, int method)
{
	switch (method)
	{
	case 0: // Box box overlap test
		return bb_overlapTest(a, b);
		break;
			
	default:
		break;
	}

	return false;
}

// 완전 탄성 충돌 가정함.
void Physics::process_collision(GameObject* a, GameObject* b)
{
	// 필요한것 질량 ( mass ) 속도 (accelation )
	float a_mass, a_vx, a_vy, a_vz;
	a_mass = a->get_mass();
	a->get_velocity(&a_vx, &a_vy, &a_vz);
	
	float afvx, afvy, afvz;

	float b_mass, b_vx, b_vy, b_vz;
	b_mass = b->get_mass();
	b->get_velocity(&b_vx, &b_vy, &b_vz);
	
	float bfvx, bfvy, bfvz;

	afvx = ((a_mass - b_mass) / (a_mass + b_mass) * a_vx)
		+ ((b_mass * 2.f) / (a_mass + b_mass)) * b_vx;

	afvy = ((a_mass - b_mass) / (a_mass + b_mass) * a_vy)
		+ ((b_mass * 2.f) / (a_mass + b_mass)) * b_vy;

	afvz = ((a_mass - b_mass) / (a_mass + b_mass) * a_vz)
		+ ((b_mass * 2.f) / (a_mass + b_mass)) * b_vz;

	bfvx = ((a_mass * 2.f) / (a_mass + b_mass)) * a_vx
		+ ((b_mass - a_mass) / (a_mass + b_mass)) * b_vx;

	bfvy = ((a_mass * 2.f) / (a_mass + b_mass)) * a_vy
		+ ((b_mass - a_mass) / (a_mass + b_mass)) * b_vy;

	bfvz = ((a_mass * 2.f) / (a_mass + b_mass)) * a_vz
		+ ((b_mass - a_mass) / (a_mass + b_mass)) * b_vz;
	
	a->set_velocity(afvx, afvy, afvz);
	b->set_velocity(bfvx, bfvy, bfvz);
}

bool Physics::bb_overlapTest(GameObject* a, GameObject* b)
{
	// bounding box를 얻어서
	Transform* a_transform = a->get_transform();
	Transform* b_transform = b->get_transform();
// a
	float apx = a_transform->position.x;
	float apy = a_transform->position.y;
	float apz = a_transform->position.z;

	float avx = a_transform->size.x;
	float avy = a_transform->size.y;
	float avz = a_transform->size.z;

// b
	float bpx = b_transform->position.x;
	float bpy = b_transform->position.y;
	float bpz = b_transform->position.z;

	float bvx = b_transform->size.x;
	float bvy = b_transform->size.y;
	float bvz = b_transform->size.z;

	float aminx, aminy, aminz;
	float amaxx, amaxy, amaxz;

	float bminx, bminy, bminz;
	float bmaxx, bmaxy, bmaxz;

	aminx = apx - avx / 2.f;
	aminy = apy - avy / 2.f;
	aminz = apz - avz / 2.f;

	amaxx = apx + avx / 2.f;
	amaxy = apy + avy / 2.f;
	amaxz = apz + avz / 2.f;

	bminx = bpx - bvx / 2.f;
	bminy = bpy - bvy / 2.f;
	bminz = bpz - bvz / 2.f;

	bmaxx = bpx + bvx / 2.f;
	bmaxy = bpy + bvy / 2.f;
	bmaxz = bpz + bvz / 2.f;
	
	// 겹치는지 확인.
	if ( aminx > bmaxx )
		return false; 
	if (amaxx < bminx)
		return false;

	if (aminy > bmaxy)
		return false;
	if (amaxy < bminy)
		return false;

	if (aminz > bmaxz)
		return false;
	if (amaxz < bminz)
		return false;

	return true;
}
