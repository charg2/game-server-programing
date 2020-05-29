#pragma once

#include <cstdint>
#include "MMOSector.h"


struct MMONear
{
	MMOSector* sectors[4];
	char	   count;
};