#pragma once

struct MoveResponse;
class HitComponent;

class GameObject;
class MainPlayer : public GameObject
{
public:
	MainPlayer();

	virtual ~MainPlayer();

	virtual void render()									override final;
	virtual void update(float elpasedTime)					override final;
	virtual void collide(float elpasedTime)					override final;
	virtual void update_after_collision(float elpasedTime)	override final;
	virtual void on_collision(GameObject* other)			override final;

	void move_using_order(char order);
	void move_using_response(MoveResponse& response);

//private:
//	bool cna_shot();
//	void reset_bullet_cooltime();
//	void shot(float, float, float);
//	void update_directioin(float, float);

	int8_t x, y, z;
	wchar_t nickname[16];

};

extern MainPlayer* g_player;

