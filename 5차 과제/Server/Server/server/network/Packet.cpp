#include "Packet.h"
#include "../util/exception.h"


//c2::concurrency::ThreadLocalMemoryPool<Packet> Packet::packet_pool;

namespace c2
{
	c2::concurrency::ThreadLocalMemoryPool<Packet, 1024, true> c2::Packet::packet_pool{};
}