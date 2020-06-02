#pragma once
#include <Windows.h>
#include <cstdint>
#include <map>

struct MMOSector;
struct MMOActor;
struct MMOZone;
struct MMONpc
{

	void update_entering_actor(MMOActor* actor);
	void update_leaving_actor(MMOActor* actor);

	void move();
	//int8_t direction = rand
	// 목표지점.. 

	
	// 길찾기 한 칸.

	// 섹터가 변경되면 바꾸기.

	// 주변에 뿌리기.

	// 주변에 아무도 없다면 

	// compare echage   is_active = 0;

	int16_t		hp;
	int16_t		max_hp;
	int32_t		x, y;
	char		name[50];
	uint64_t	id;
	MMOSector*	current_sector;
	MMOActor*	target;
	MMOZone*		zone;
	uint64_t	is_active;

	std::map<int32_t, MMOActor*>	view_list;
	SRWLOCK							lock;
};

