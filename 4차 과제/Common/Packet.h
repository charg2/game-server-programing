#ifndef C2_PACKET_H
#define C2_PACKET_H

#include <cstdint>
#include <type_traits>
#include <vcruntime_string.h>

//#include <string>
//#include <list>
#include "exception.h"
#include "MemoryPoolTLS.h"

//#include "Profiler.h"

#define Out


namespace c2
{
	constexpr size_t kMaximumSegmentSize = 1460;

	class Packet
	{
	public:
		Packet();
		Packet(const Packet& other);
		Packet(Packet&& other) noexcept;
		~Packet();

		Packet& operator=(const Packet& other);
		Packet& operator=(Packet&& other) noexcept;

		void clear();
		char* get_buffer() const noexcept;

		void rewind() noexcept;
		void resize(size_t length);

		size_t capacity() const noexcept;
		size_t size() const noexcept;

		void move_write_head(size_t length);
		void move_read_head(size_t length);

		void write(const void* src, size_t size);
		void read(Out void* src, size_t size);
		void peek(Out void* src, size_t size);

		void reset();

		// primitive types version 
		//template<typename T>
		//Packet& operator<<(const T& src);
		////template<typename T>
		////Packet& operator<<(T&& src);
		//template<typename T>
		//Packet& operator>>(Out T& dest);

		static Packet* alloc();
		static void free(Packet* packet);

	private:
		char*	buffer;
		size_t	payload_capacity;

		size_t	write_head;
		size_t	read_head;

		/*int64_t ref_count;
		int64_t release_flag;*/

		static inline c2::concurrency::MemoryPoolTLS<Packet> packet_pool;
	};
}
#endif
