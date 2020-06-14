#pragma once

#include "MMOSector.h"


struct MMONear
{
	MMOSector* sectors[4];
	char	   count;

	bool operator==( const MMONear& other)
	{
		for ( int n =0; n < 4; ++n  )
		{
			if (other.sectors[n] != this->sectors[n])
			{
				return false;
			}
		}
	
		return true;
	}
};