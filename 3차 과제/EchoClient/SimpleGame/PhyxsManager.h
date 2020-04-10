#pragma once

class GameObject;
class Physics
{
public:
	Physics();
	~Physics();

	bool is_overlapped(GameObject* a, GameObject* b, int method = 0);
	void process_collision(GameObject* a, GameObject* b);

private:
	bool bb_overlapTest(GameObject* a, GameObject* b);
};

