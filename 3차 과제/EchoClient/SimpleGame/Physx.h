#pragma once
#include "Vector3.h"


class Physx
{
public:
	Physx();
	Physx(const Physx& other);
	void initialize();

	Vector3&		get_velocity();
	Vector3&		get_acceleration();
	const Vector3&	get_cvelocity() const;
	const Vector3&	get_cacceleration() const;

	float			get_frictioncoef();
	float			get_mass();

	void			set_velocity(Vector3& velocity);
	void			set_velocity(float vx, float vy, float vz);

	void			set_acceleration(Vector3& acceleration);
	void			set_acceleration(float ax, float ay, float az);

	void			set_frictioncoef(float coef);
	void			set_mass(float mass);

private:
	Vector3 velocity;
	Vector3 acceleration;
	float	mass;
	float	friction_coefficient; //¸¶Âû °è¾¥friction co
};