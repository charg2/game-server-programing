#pragma once

#include <cstdint>
#include "MMOSector.h"


struct MMONear
{
	MMOSector* sectors[4];
	char	   count;

	//bool operator==( const MMONear& other)
	//{
	//	int same_count = 0;
	//	for (MMOSector* my_sector : this->sectors)
	//	{
	//		for (MMOSector* other_sector : other.sectors)
	//		{
	//			if ( my_sector != nullptr && other_sector != nullptr && my_sector == other_sector)
	//			{
	//				same_count += 1;
	//			}
	//		}
	//	}

	//	return same_count == this->count;
	//}

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