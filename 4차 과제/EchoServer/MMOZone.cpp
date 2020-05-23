#include "MMOZone.h"

MMOZone::~MMOZone()
{
}

void MMOZone::dispatch_network()
{
	for (;;)
	{
		void* message;
		if (message_queue.try_pop(message))
		{
			message;
		}
		else
		{
			break;
		}
	}

}

void MMOZone::simulate_interaction()
{
}

void MMOZone::broadcaset_all()
{
}

void MMOZone::broadcaset_nearby_others()
{
}

void MMOZone::process_packet()
{
}
