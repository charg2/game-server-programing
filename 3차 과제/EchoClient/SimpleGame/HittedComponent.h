#pragma once

class GameObject;
class HitComponent
{
public:
	HitComponent(GameObject* owner, float recovery_time = 0.2f);
	~HitComponent();
	
	bool can_hitted();
	void update(float elapsed_time);
	void hit();

private:
	GameObject* owner;
	float		hit_timer;
	float		hit_recovery_time;
	bool		is_hitted;
};

