#pragma once
class GameObject;
class GameSceneBG1 final : public GameObject
{
public:
	GameSceneBG1();
	~GameSceneBG1();

	virtual void render() final override;
	virtual void update(float elpasedTime)	final override;
	virtual void collide(float elpasedTime)	final override;
	virtual void update_after_collision(float elpasedTime) final override;
	virtual void on_collision(GameObject*) override final;

private:
	int shading_texture_id;
};

