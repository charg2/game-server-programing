#include "../C2Server/C2Server/pre_compile.h"
#include "MMOActor.h"
#include "MMOZone.h"

MMOActor::MMOActor()
	:
x{}, y{},
name{},
session_id{},
current_sector{}, prev_sector{},
zone{}, simulator{}
{

}

void MMOActor::move_to(int32_t x, int32_t y)
{
	this->x = x; 
	this->y = y;

	current_sector;
}

void MMOActor::reset()
{
}
