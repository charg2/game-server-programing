#pragma once
#include "Vector3.h"

struct Transform
{
public:
	Transform();
	Transform(const Transform& other);
	void initialize();

	Vector3& get_position();
	Vector3& get_size();
	Vector3& get_scale();
	Vector3& get_rotation();

	void set_position(Vector3&);
	void set_size(Vector3&);
	void set_scale(Vector3&);
	void set_rotation(Vector3&);

	void set_position(float, float, float);
	void set_size(float, float, float);
	void set_scale(float, float, float);
	void set_rotation(float, float, float);

	Vector3 position;
	Vector3 size;
	Vector3 scale;
	Vector3 rotation;
};

