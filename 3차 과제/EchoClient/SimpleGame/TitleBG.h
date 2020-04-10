#pragma once

class GameObject;
class TitleBG final : public GameObject
{
public:
	TitleBG();
	~TitleBG();

	virtual void render()									final override;
	virtual void update(float elpasedTime)					final override;
	virtual void collide(float elpasedTime)					final override;
	virtual void update_after_collision(float elpasedTime)	final override;
	virtual void on_collision(GameObject*)					override final;
private:
	int title;
	int title1;
	int current_texture;

};

