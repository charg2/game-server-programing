#pragma once
class Scene;
class SceneManager;
class GameScene1 final : public Scene
{
public:
	GameScene1(SceneManager* scenemanager);
	virtual ~GameScene1();

	virtual void on_change_scene() override final;
private:
	int bgm;
};

