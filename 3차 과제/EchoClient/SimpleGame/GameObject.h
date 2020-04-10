#pragma once
#include "PhyxsManager.h"
#include <string>
#include <unordered_map>

class	Renderer;
struct	Transform;
class	Physx;
class	Scene;
class	Layer;
class	HitComponent;

class GameObject
{
public:
	GameObject();
	GameObject(const GameObject& other);
	GameObject(GameObject&& other) noexcept;
	GameObject(Transform& transform, FColor& color);

	virtual ~GameObject();

	virtual void render();
	virtual void update(float elpasedTime);
	virtual void collide(float elpasedTime);
	virtual void update_after_collision(float elpasedTime);
	virtual void on_collision(GameObject* other);

	void get_position(float *x, float *y, float *z);
	void get_velocity(float* vx, float* vy, float* vz);
	void get_acceleration(float* ax, float* ay, float* az);
	void getColor(float* r, float* g, float* b, float* a);

	FColor*			get_color();
	Transform*		get_transform();
	Physx*			get_physx();

	void			getFricCoef(float* coef);
	void			get_tag(Tag* obj_type);
	Tag				get_tag();
	void			getTextureID(int* id);
	void			setTextureID(int id);
	float			get_age();
	int				get_damage();
	float			get_mass();
	Layer*			get_layer();
	State			get_state();
	float			get_hp();
	Renderer*		get_renderer() const;
	std::wstring&	get_name();

	void	set_hp(float hp);
	void	set_position(float x, float y, float z);
	void	set_size(float sx, float sy, float sz);
	void	set_scale(float scx, float scy, float scz);
	void	set_velocity(float vx, float vy, float vz);
	void	set_acceleration(float ax, float ay, float az);
	void	set_color(float r, float g, float b, float alpha);
	void	set_mass(float mass);
	void	set_FricCoef(float coef);
	void	set_type(Tag type);
	void	set_age(float time);
	void	set_state(State state);
	void	set_parent(GameObject* parent);
	void	set_scene(Scene* scn);

	void	calc_damage(float damage);
	void	add_force(float x, float y, float z, float);

	bool	is_parent(GameObject* obj);
	bool	can_hitted();

	void	set_active(bool active);
	bool	get_active();
	void	set_name(std::wstring& name);
	void	set_layer(Layer*);
	void	set_renderer(Renderer* renderer);
	void	hit();
	static GameObject*	find_object(std::wstring* name);
	static bool			find_objects(Tag tag, Out std::list<GameObject*>& out_list);
	static void			destroy(GameObject* gobj);

protected:
	Transform		transform;
	FColor			color;
	Physx*			physx;
	Layer*			current_layer;
	Scene*			current_scene;
	Renderer*		renderer;
	GameObject*		parent;
	Tag				tag; 
	std::wstring	name;

	State			state;
	bool			active_self;
	int				texture_id;
	HitComponent*	hit_component;
// rendered
	int				age;
	int				damage;
// contents
	float health_point;
	float max_health_point;

	static std::unordered_map<std::wstring*, GameObject*> name_map;
	static std::unordered_multimap<Tag, GameObject*>	  group_map;
};

