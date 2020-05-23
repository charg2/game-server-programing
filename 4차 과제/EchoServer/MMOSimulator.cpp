#include "../C2Server/C2Server/pre_compile.h"
#include "MMOSimulator.h"
#include "MMOMessage.h"

MMOSimulator& MMOSimulator::get_instance()
{
	MMOSimulator instance;
	return instance;
}

void MMOSimulator::dispatch()
{
	MMOMessage* msg;
	for (;;)
	{
		if (message_queue.try_pop(msg))
		{

		}

		// 

		switch (1)
		{
			case c2::enumeration::C2S_LOGIN:
			{
				
			}

			case c2::enumeration::C2S_MOVE:
			{

			}
		}
	}
}

void MMOSimulator::simulate()
{
	// 변환점을 확인하고 update
	zones.simulate_interaction();


	// 가비지 컬렉션.
}

void MMOSimulator::put_message(MMOMessage* message)
{
	message_queue.push(message);
}
