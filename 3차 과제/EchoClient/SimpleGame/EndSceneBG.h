#pragma once

class GameObject;
class EndSceneBG final : public GameObject
{
public:
	EndSceneBG();
	~EndSceneBG();

	virtual void render() final override;
	virtual void update(float elpasedTime)	final override;
	virtual void collide(float elpasedTime)	final override;
	virtual void update_after_collision(float elpasedTime) final override;
};

