#pragma once
#include <cstdint>
//#include <type_traits>
#include <vcruntime_string.h>
#include "../util/exception.h"
#include "../concurrency/MemoryPoolTLS.h"

#define Out


namespace c2
{
	constexpr size_t kMaximumSegmentSize = 1460;

	class Packet
	{
	public:
		Packet() : buffer{ nullptr }, payload_capacity{ kMaximumSegmentSize }, write_head{ 0 }, read_head{ 0 }//, ref_count{ 0 }
		{
			if (INVALID_HANDLE_VALUE == Packet::heap_handle)
			{
				Packet::heap_handle = HeapCreate( /* HEAP_ZERO_MEMORY */ HEAP_GENERATE_EXCEPTIONS, 0, NULL);
			}

			//buffer = new char[kMaximumSegmentSize];
			buffer = (char*)HeapAlloc(heap_handle, HEAP_GENERATE_EXCEPTIONS, kMaximumSegmentSize);

			//new char[kMaximumSegmentSize];
		}
		//Packet(const Packet& other);
		//Packet(Packet&& other) noexcept;
		~Packet()
		{
#ifdef PROFILE_ON
			PROFILE_FUNC;
#endif
			//release();
			HeapFree(heap_handle, 0, buffer);
			//delete[] buffer;

			read_head = 0;
			write_head = 0;
		}

		//Packet& operator=(const Packet& other);
		//Packet& operator=(Packet&& other) noexcept;

		void clear()
		{
#ifdef PROFILE_ON
			PROFILE_FUNC;
#endif
			write_head = 0;
			read_head = 0;
		}

		char* get_buffer() const noexcept
		{
			return buffer;
		}

		void rewind() noexcept
		{
			this->write_head = this->read_head = 0;
		}

		void resize(size_t length)
		{
			char* new_buffer = new char[length] {};
			size_t new_capacity = length > payload_capacity ? payload_capacity : length;

			memcpy(new_buffer, this->buffer, new_capacity);

			delete[] this->buffer;

			this->buffer = new_buffer;
			this->payload_capacity = length;
			this->write_head = length > this->write_head ? this->write_head : length;
			this->read_head = length > this->read_head ? this->read_head : length;
		}


		size_t capacity() const noexcept
		{
#ifdef PROFILE_ON
				PROFILE_FUNC;
#endif
				return payload_capacity;
		}


		size_t size() const noexcept
		{
#ifdef PROFILE_ON
			PROFILE_FUNC;
#endif
			return write_head;
		}

		void move_write_head(size_t length)
		{
			write_head += length;
		}

		void move_read_head(size_t length)
		{
			read_head += length;
		}

		void write(const void* data, size_t size)
		{
#ifdef PROFILE_ON
			PROFILE_FUNC;
#endif
			if (write_head + size >= payload_capacity) // idx니깐 같아져도 문제가 생김.
			{
				size_t new_payload_capacity = payload_capacity * 2;
				size_t new_size = write_head + size > new_payload_capacity ? write_head + size : new_payload_capacity;

				resize(new_size);
			}

			// memcpy 때리는거보다.
			switch (size)
			{
			case 0:
				break;
			case 1:
				this->buffer[write_head] = *(uint8_t*)data;
				break;

			case 2:
				*(uint16_t*)(&this->buffer[write_head]) = *(uint16_t*)data;
				break;

			case 3:
				this->buffer[write_head] = *(uint8_t*)data;
				*(uint16_t*)(&this->buffer[write_head + 1]) = *((uint16_t*)((uint8_t*)data + 1));
				break;

			case 4:
				*(uint32_t*)(&this->buffer[write_head]) = *(uint32_t*)data;
				break;

			case 5:
				this->buffer[write_head] = *(uint8_t*)data;
				*(uint32_t*)(&this->buffer[write_head + 1]) = *((uint32_t*)((uint8_t*)data + 1));
				break;

			case 6:
				*(uint16_t*)(&this->buffer[write_head]) = *(uint16_t*)data;
				*(uint32_t*)(&this->buffer[write_head + 2]) = *((uint32_t*)((uint8_t*)data + 2));
				break;

			case 7:
				this->buffer[write_head] = *(uint8_t*)data;
				*(uint16_t*)(&this->buffer[write_head + 1]) = *((uint16_t*)((uint8_t*)data + 1));
				*(uint32_t*)(&this->buffer[write_head + 3]) = *((uint32_t*)((uint8_t*)data + 3));
				break;

			case 8:
				*(uint64_t*)(&this->buffer[write_head]) = *(uint64_t*)data;
				break;

			default: // 8 초과하는 데이터를이렇게 받는다.
				memcpy(&buffer[write_head], data, size); // 현재 인덱스에 복사를 함.
				break;
			}

			write_head += size;
		}

		void read(Out void* data, size_t size)
		{
			//#ifdef PROFILE_ON
			//		PROFILE_FUNC;
			//#endif
			if (read_head + size > write_head) // idx니깐 같아져도 문제가 생김.
			{
				// 이건 무족너 문제 있는 것.
				// error handling... 
				c2::util::assert_if_false(false);
			}


			// 1 2 4  8 바이트 읽기에 대해서도 최적화를 해준다.
			switch (size)
			{
			case 0:
				break;

			case 1:
				*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
				break;

			case 2:
				*(uint16_t*)data = *(uint16_t*)&this->buffer[read_head];
				break;

			case 3:
				*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
				*((uint16_t*)((uint8_t*)data + 1)) = *(uint16_t*)&this->buffer[read_head + 1];
				break;

			case 4:
				*(uint32_t*)data = *(uint32_t*)&this->buffer[read_head];
				break;

			case 5:
				*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
				*((uint32_t*)((uint8_t*)data + 1)) = *(uint32_t*)&this->buffer[read_head + 1];
				break;

			case 6:
				*(uint16_t*)(data) = *(uint16_t*)&this->buffer[read_head];
				*((uint32_t*)((uint8_t*)data + 2)) = *(uint32_t*)&this->buffer[read_head + 2];
				break;


			case 7:
				*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
				*((uint16_t*)((uint8_t*)data + 1)) = *(uint16_t*)&this->buffer[read_head + 1];
				*((uint32_t*)((uint8_t*)data + 3)) = *(uint32_t*)&this->buffer[read_head + 3];
				break;

			case 8:
				*(uint64_t*)data = *(uint64_t*)&this->buffer[read_head];
				break;

			default: // 8 초과하는 데이터를이렇게 받는다.
				//memcpy(&buffer[write_head], data, size); // 현재 인덱스에 복사를 함.
				memcpy(data, &buffer[read_head], size);
				break;
			}

			read_head += size;
		}
		void peek(Out void* data, size_t size)
		{
			if (read_head + size > write_head)
			{
				c2::util::crash_assert();
			}

			// 1 2 4  8 바이트 읽기에 대해서도 최적화를 해준다.
			switch (size)
			{
			case 0:
				break;

			case 1:
				*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
				break;

			case 2:
				*(uint16_t*)data = *(uint16_t*)&this->buffer[read_head];
				break;

			case 3:
				*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
				*((uint16_t*)((uint8_t*)data + 1)) = *(uint16_t*)&this->buffer[read_head + 1];
				break;

			case 4:
				*(uint32_t*)data = *(uint32_t*)&this->buffer[read_head];
				break;

			case 5:
				*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
				*((uint32_t*)((uint8_t*)data + 1)) = *(uint32_t*)&this->buffer[read_head + 1];
				break;

			case 6:
				*(uint16_t*)(data) = *(uint16_t*)&this->buffer[read_head];
				*((uint32_t*)((uint8_t*)data + 2)) = *(uint32_t*)&this->buffer[read_head + 2];
				break;

			case 7:
				*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
				*((uint16_t*)((uint8_t*)data + 1)) = *(uint16_t*)&this->buffer[read_head + 1];
				*((uint32_t*)((uint8_t*)data + 3)) = *(uint32_t*)&this->buffer[read_head + 3];
				break;

			case 8:
				*(uint64_t*)data = *(uint64_t*)&this->buffer[read_head];
				break;

			default: // 8 초과하는 데이터를이렇게 받는다.
				//memcpy(&buffer[write_head], data, size); // 현재 인덱스에 복사를 함.
				memcpy(data, &buffer[read_head], size);
				break;
			}

		}

		void reset()
		{
			write_head = read_head = 0;
		}


		// primitive types version 
		//template<typename T>
		//Packet& operator<<(const T& src);
		////template<typename T>
		////Packet& operator<<(T&& src);
		//template<typename T>
		//Packet& operator>>(Out T& dest);

		static Packet* alloc()
		{
			return packet_pool.alloc();
		}
		static void free(Packet* packet)
		{
			packet_pool.free(packet);
		}




	private:
		char*	buffer;
		size_t	payload_capacity;

		size_t	write_head;
		size_t	read_head;

		/*int64_t ref_count;
		int64_t release_flag;*/

		static inline c2::concurrency::MemoryPoolTLS<Packet> packet_pool{};
		static inline HANDLE heap_handle { INVALID_HANDLE_VALUE };
	};
}
