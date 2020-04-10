#pragma once
class GameObject;
class TitleMent final : public GameObject
{
public:
	TitleMent();
	virtual ~TitleMent();

	virtual void render()									override final;
	virtual void update(float elapsed_time)					override final;
	virtual void collide(float elapsed_time)				override final;
	virtual void update_after_collision(float elapsed_time)	override final;
	virtual void on_collision(GameObject*)					override final;

private:
	void swap_texture();

private:
	bool				space_pressed;
	std::vector<int>	texture_vector;
	int					current_texture;
	float				animation_timer;
};

