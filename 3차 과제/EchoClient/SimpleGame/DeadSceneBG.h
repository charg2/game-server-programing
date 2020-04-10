#pragma once

class GameObject;
class DeadSceneBG final : public GameObject
{
public:
	DeadSceneBG();
	~DeadSceneBG();

	virtual void render() final override;
	virtual void update(float elpasedTime)	final override;
	virtual void collide(float elpasedTime)	final override;
	virtual void update_after_collision(float elpasedTime) final override;
};

